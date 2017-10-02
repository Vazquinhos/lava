#include "Window.h"
#include "Renderer.h"
#include "Debug.h"
#include <iostream>

int main()
{
    lava::Window window("window", 400, 300);
    std::vector< std::string > extensions;
    std::vector< std::string > layers;
    lava::setOutputMessageFunction
    (
        [](lava::MessageSeverity _severity, const std::string& _msg)
        {
            std::string fullMsg = lava::to_string(_severity) + " : " + _msg + "\n";
            OutputDebugStringA(fullMsg.c_str());
        }
    );
    lava::Renderer lavaRenderer( &window, extensions,layers);
    while (!window.IsClosed())
      lavaRenderer.Update();
    return 0;
}
