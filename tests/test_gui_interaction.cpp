#include "GuiWidgets.h"
#include <iostream>
#include "imgui.h"

namespace GuiInteractionTests {
    int g_tests_passed = 0;
    int g_tests_failed = 0;

    void Run() {
        std::cout << "\n=== Gui Interaction Tests ===" << std::endl;

        IMGUI_CHECKVERSION();
        ImGuiContext* ctx = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        
        // Mock a font to avoid assertion in some ImGui versions
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        // Test 1: Decorator Execution
        {
            float val = 0.5f;
            bool decoratorCalled = false;
            ImGui::NewFrame();
            ImGui::Columns(2);
            GuiWidgets::Float("TestDecorator", &val, 0.0f, 1.0f, "%.2f", nullptr, [&](){ decoratorCalled = true; });
            ImGui::EndFrame();
            
            if (decoratorCalled) {
                std::cout << "[PASS] Float Decorator Execution" << std::endl;
                g_tests_passed++;
            } else {
                std::cout << "[FAIL] Float Decorator NOT executed" << std::endl;
                g_tests_failed++;
            }
        }

        // Test 2: Result Struct Defaults
        {
            GuiWidgets::Result res;
            if (!res.changed && !res.deactivated) {
                std::cout << "[PASS] Result default values" << std::endl;
                g_tests_passed++;
            } else {
                std::cout << "[FAIL] Result default values incorrect" << std::endl;
                g_tests_failed++;
            }
        }

        // Note: Full Arrow Key / Hover interaction testing is better suited for E2E tests
        // with a real window/event loop. Basic Logic and Decorator execution verified above.

        ImGui::DestroyContext(ctx);
    }
}
