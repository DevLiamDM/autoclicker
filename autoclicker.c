#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// Variabile globale per controllare l'esecuzione
volatile bool running = false;

// Callback per l'Event Tap
CGEventRef eventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {
    if (type == kCGEventKeyDown) {
        CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
        // Verifica se il tasto premuto è "p"
        if (keycode == 35) { // 35 è il keycode per "p" su tastiere ANSI
            running = !running; // Inverte lo stato di esecuzione
        }
    }
    return event;
}
p
// Funzione per inizializzare l'Event Tap
void* keyboardListener(void* arg) {
    CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef eventTap = CGEventTapCreate(kCGSessionEventTap,
                                              kCGHeadInsertEventTap,
                                              0,
                                              eventMask,
                                              eventCallback,
                                              NULL);
    if (!eventTap) {
        fprintf(stderr, "Impossibile creare l'Evenpt Tap.\n");
        return NULL;
    }

    CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);p
    CGEventTapEnable(eventTap, true);
    CFRunLoopRun();

    return NULL;
}

// Func Main
int main() {
    // Crea il thread per ascoltare l'input da tastiera
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, keyboardListener, NULL);

    // Intervallo tra i clic in microsecondi (Ora inpostato 30 cps)
    useconds_t interval = 30000;

    while (true) {
        if (running) {
            // Ottiene la posizione attuale del mouse
            CGEventRef ourEvent = CGEventCreate(NULL);
            CGPoint mouseLoc = CGEventGetLocation(ourEvent);
            CFRelease(ourEvent);

            // Crea gli eventi di pressione e rilascio del clic sinistro
            CGEventRef click_down = CGEventCreateMouseEvent(
                NULL, kCGEventLeftMouseDown, mouseLoc, kCGMouseButtonLeft
            );
            CGEventRef click_up = CGEventCreateMouseEvent(
                NULL, kCGEventLeftMouseUp, mouseLoc, kCGMouseButtonLeft
            );

            // Esegue gli eventi
            CGEventPost(kCGHIDEventTap, click_down);
            CGEventPost(kCGHIDEventTap, click_up);

            // Libera la memoria
            CFRelease(click_down);
            CFRelease(click_up);
        }
        usleep(interval);
    }

    return 0;
}
