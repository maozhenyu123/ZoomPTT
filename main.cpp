#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Audio.hpp>
#include <Carbon/Carbon.h>
#include <libproc.h>
#include <time.h>
#include <iostream>
#include <chrono>
#include <thread>

const int TONE_SAMPLE_RATE = 44100;
const double TONE_DURATION_IN_SEC = 0.05;

class Main {
    private:
      sf::SoundBuffer buffer;
      sf::SoundBuffer buffer2;
      sf::Sound sound;
      sf::Sound sound2;
      pid_t zoom_pid;
      time_t last_update_zoom_pid;
      void postKeyboardEvent(CGKeyCode virtualKey, bool keyDown, CGEventFlags flags);

      // ~Main();
    public:
      bool prepareToneSound();
      void onKeyUp();
      void onKeyDown();
      Main();
};

pid_t find_pids(const char *name)
{
    pid_t pids[2048];
    int b = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    int n = b / sizeof(pids[0]);
    for (int i = 0; i < n; i++) {
        struct proc_bsdinfo proc;
        int st = proc_pidinfo(pids[i], PROC_PIDTBSDINFO, 0,
                             &proc, PROC_PIDTBSDINFO_SIZE);
        if (st == PROC_PIDTBSDINFO_SIZE) {
            if (strcmp(name, proc.pbi_name) == 0) {
               return pids[i];     
            }
        }       
    }
}

Main::Main() {

}

void Main::postKeyboardEvent(CGKeyCode virtualKey, bool keyDown, CGEventFlags flags)
{
    CGEventSourceRef source = CGEventSourceCreate(kCGEventSourceStatePrivate);
    CGEventRef push = CGEventCreateKeyboardEvent(source, virtualKey, keyDown);
    CGEventSetFlags(push, flags);
    // CGEventPost(kCGHIDEventTap, push); // send globally
    if (this->zoom_pid == 0 || this->last_update_zoom_pid < time(NULL) - 20) {
        this->zoom_pid = find_pids("zoom.us");
        this->last_update_zoom_pid = time(NULL);
        std::cout << "Found zoom; pid = " << this->zoom_pid << std::endl;
    }
    CGEventPostToPid(this->zoom_pid, push);
    CFRelease(push);
    CFRelease(source);
}

bool generateTone(sf::SoundBuffer& bf, double tone_frequency) {
    // generate sine wave
    int sample_count = (int)(TONE_DURATION_IN_SEC * TONE_SAMPLE_RATE);
    short samples[sample_count];
    for (int sample_i = 0; sample_i < sample_count; ++sample_i) 
    {
        double t = sample_i / (double)(TONE_SAMPLE_RATE);
        double amp = sin(t * tone_frequency * M_PI);

        short sample_value = (short)(amp * std::numeric_limits<short>::max());
        samples[sample_i] = sample_value;
    }
    if (!bf.loadFromSamples(samples, sample_count, 1, TONE_SAMPLE_RATE))
    {
        return false;
    }
}

bool Main::prepareToneSound(){
    generateTone(this->buffer, 700);
    generateTone(this->buffer2, 800);
    this->sound.setBuffer(this->buffer);
    this->sound2.setBuffer(this->buffer2);
    std::cout << "TONE IS READY" << std::endl;
    return true;
}

void Main::onKeyDown() {
    this->sound2.play();
    // PostKeyboardEvent(kVK_ANSI_P, true, kCGEventFlagMaskControl | kCGEventFlagMaskCommand);
    // PostKeyboardEvent(kVK_ANSI_P, false, kCGEventFlagMaskControl | kCGEventFlagMaskCommand);
    this->postKeyboardEvent(kVK_Space, true, kCGEventFlagMaskNonCoalesced);
    std::cout << "Key Down" << std::endl;
    // Load a music to play
}

void Main::onKeyUp() {
    this->postKeyboardEvent(kVK_Space, false, kCGEventFlagMaskNonCoalesced);
    // PostKeyboardEvent(kVK_ANSI_P, true, kCGEventFlagMaskControl | kCGEventFlagMaskCommand);
    // PostKeyboardEvent(kVK_ANSI_P, false, kCGEventFlagMaskControl | kCGEventFlagMaskCommand);
    std::cout << "Key Up" << std::endl;
    this->sound.play();
}

int main() {
    auto isButtonPressed = false;
    std::cout << "Zhenyu's ZOOM Controller" << std::endl;
    auto m = new Main();
    if(!m->prepareToneSound()) {
        std::cout << "Hmmm, tone sound can not be loaded" << std::endl;
    }
    // guess what?? it's hard-coded
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        sf::Joystick::update();
        if (!sf::Joystick::isConnected(0)) {
            continue;
        }
        auto pressed = sf::Joystick::isButtonPressed(0, 0);
        if (!isButtonPressed && pressed) {
            // pressed
            isButtonPressed = true;
            m->onKeyDown();
        } else if (isButtonPressed && !pressed) {
            isButtonPressed = false;
            m->onKeyUp();
        }
        // for (int i = 0; i < sf::Joystick::Count; i++) {
        //     if (!sf::Joystick::isConnected(i)) {
        //         continue;
        //     }
        //     for (int j = 0; j < sf::Joystick::getButtonCount(i); j++) {
        //         if (sf::Joystick::isButtonPressed(i, j)) {
        //             // SET PTT
        //             std::cout << "OK" << i << ", " << j << std::endl;
        //         }
        //     }
        // }
    }
    return 0;
}