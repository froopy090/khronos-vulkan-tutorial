#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN // glfw is what creates a window to see stuff
#include <GLFW/glfw3.h> //this automatically include the vulkan headers with it

#include <cstdlib>
#include <exception>
#include <iostream>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
/*Vulkan uses validation layers for debugging rather than having built in error
 * messages*/
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Triangle {
public:
  void run() {
    // THE ORDER IN WHICH THESE ARE DONE IS IMPORTANT
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  // class members
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  // class functions
  void initVulkan() { 
    createInstance();
    setupDebugMessenger();
  }

  void setupDebugMessenger(){
    if(enableValidationLayers){return;}
  }

  void createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{}; // Vulkan struct
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    /*uint32_t glfwExtensionCount = 0;*/
    /*const char **glfwExtensions;*/
    /**/
    /*glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);*/
    /*std::cout << "glfw extensions" << std::endl;*/
    /*for (const auto& instanceExtensions : extensions) {*/
    /*  std::cout << '\t' << instanceExtensions << std::endl;*/
    /*}*/

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    /*retrieve a list of supported extensions before creating an instance i.e.
     * checking for extension support*/
    /*uint32_t extensionCount = 0;*/
    /*vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
     * nullptr);*/
    /*std::vector<VkExtensionProperties> supportedExtensions(extensionCount);*/
    /**/
    /*vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,*/
    /*                                       supportedExtensions.data());*/
    /**/
    /*std::cout << "available extensions" << std::endl;*/
    /*for (const auto &extension : supportedExtensions) {*/
    /*  std::cout << '\t' << extension.extensionName << std::endl;*/
    /*}*/
    /*end of supported extensions list*/

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }

    /*if (extensionCheck(glfwExtensions, glfwExtensionCount,
     * supportedExtensions)) {*/
    /*  std::cout << "all glfw extensions are present in the "*/
    /*               "available extensions list"*/
    /*            << std::endl;*/
    /*}*/
  }

  /*challenge: checks if all the extensions returned by
   * glfwGetRequiredInstanceExtensions are included in the supported extensions
   * list, returns false if there is at least one extension in the glfw group
   * that is not in the supported extensions*/
  bool extensionCheck(const char **glfwExtensions, uint32_t glfwExtensionCount,
                      std::vector<VkExtensionProperties> supportedExtensions) {
    bool returnValue = true;
    // checking if glfwExtensions is even valid
    if (glfwExtensionCount == 0 || glfwExtensions == NULL) {
      return false;
    }

    for (int i = 0; i < glfwExtensionCount; i++) {
      for (const auto &extension : supportedExtensions) {
        if (strcmp(glfwExtensions[i], extension.extensionName) == 0) {
          returnValue = true;
          break;
        } else {
          returnValue = false;
        }
      }
    }
    return returnValue;
  }

  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
        if (std::strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }
      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {

    std::cerr << "validation layer:" << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  void initWindow() {
    glfwInit(); // initializes the library

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // NOT using an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  void cleanup() {
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
  }
};

int main() {
  Triangle app;

  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
