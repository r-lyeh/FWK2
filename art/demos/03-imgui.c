#include "engine.h"

int main() {
    window_create(0.75, 0);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;

    // Main loop
    while (window_swap()) {

        ui_demo(0);

        void ifd_demo();
        ifd_demo();

#if 0
        void igNodeFlowDemo();
        igNodeFlowDemo();
#endif

        static float knob = 0.5;
        igKnob("my knob", &knob, 10);

        static int wopen = 1;
        if( ui_window( "my window", &wopen) )
        {
            static float f = 3.14159;
            if( ui_float("my float", &f) );
            if( ui_button("my button") );
            ui_window_end();
        }

            #if 1
            if( igBegin("Menu", NULL, ImGuiWindowFlags_MenuBar) ) {
                if (igBeginMenuBar())
                {
                    if (igBeginMenu("File",1/*enabled*/))
                    {
                        if (igMenuItemEx("Some menu item",NULL/*icon*/,NULL/*shortcut*/,false/*selected*/,1/*enabled*/)) {}
                        igEndMenu();
                    }
                    igEndMenuBar();
                }
            }
            igEnd();
            #endif

            if (igBeginMainMenuBar()) {
                if (igBeginMenu("System",1/*enabled*/)) {
                    if (igMenuItemEx("Exit","ICON","CTRL+Q",0,1)) {
                        exit(0);
                    }
                    igEndMenu();
                }
                igSeparator();
                igText("%s", "Some Text");
                igEndMainMenuBar();
            }


        if(show_demo_window)
        igShowDemoWindow(&show_demo_window);

        if(show_demo_window)
        igShowImPlotDemoWindow(&show_demo_window);

        // You can invoke cimgui functions directly too
        {
            static float f = 0.0f;
            static int counter = 0;

            static bool wopen = 1;
            igBegin("Hello, world!", &wopen, 0);                          // Create a window called "Hello, world!" and append into it.

            igText("This is some useful text.");               // Display some text (you can use a format strings too)
            igCheckbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            igCheckbox("Another Window", &show_another_window);

            igSliderFloat("float", &f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);            // Edit 1 float using a slider from 0.0f to 1.0f

            if (igButton("Button",ImVec2(0,0)))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            igSameLine(0,0);
            igText("counter = %d", counter);

            igEnd();

            // Show another simple window.
            if (show_another_window)
            {
                igBegin("Another Window", &show_another_window, 0);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                igText("Hello from another window!");
                if (igButton("Close Me",ImVec2(0,0)))
                    show_another_window = false;
                igEnd();
            }
        }
    }

    return 0;
}

const char *appname = __FILE__;
