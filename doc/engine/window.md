# Window

The window created by [GLFW](https://www.glfw.org/) is stored in the `Window` class.

```cpp
class Window {
public:
    Window(int width, int height);

    bool should_close();
    VkExtent2D get_extent();
    void create_surface(VkInstance instance, VkSurfaceKHR* surface);
};
```

## Usage
```cpp
Window(int width, int height)
```
Constructor for the `Window` class
#### Parameters
* `w` The width of the screen
* `h` the height of the screen

$~$

```cpp
bool should_close()
```
If the window is closed or not
#### Returns
* Value of the close flag of the window

$~$

```cpp
VkExtent2D get_extent()
```
Gets the dimensions of the window 
#### Returns
* Width and height of the window in a `VkExtent2D` struct

$~$

```cpp
void create_surface(VkInstance instance, VkSurfaceKHR* surface)
```
Creates a vulkan surface for a specific window
#### Parameters
* `instance` The vulkan instance
* `surface` The handle for the surface
