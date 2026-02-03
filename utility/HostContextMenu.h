//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {

//==============================================================================
template<typename Derived>
class HostContextMenu
{
public:
  //==============================================================================
  /**
   * @brief Default constructor.
   *
   * Initializes the scaling references.
   */
  HostContextMenu() = default;

  //==============================================================================
  /**
   * @brief Destructor.
   */
  virtual ~HostContextMenu() = default;

  //==============================================================================
  /**
   * @brief Show the context menu for this component.
   *
   * This function is called to display the context menu when the user
   * right-clicks on the component. It can be overridden by derived classes to
   * provide custom context menu behavior. It takes a reference to the Parameter
   * that is the target of the context menu.
   */
  void showContextMenu(juce::AudioProcessorParameter* target)
  {
    // Check if we are actually a valid component
    if (getSelf() == nullptr) {
      jassertfalse; // This should never happen
      return;
    }

    // Search the editor in the hierarchy
    const auto* editor = findEditor<juce::AudioProcessorEditor>();
    if (editor == nullptr) {
      jassertfalse; // Could not find the editor in the hierarchy
      return;
    }

    // Find the mouse position
    auto position = editor->getMouseXYRelative().toFloat();

    // Try to get the scale from Scaleable, if available
    float scale = 1.0f;
    if (auto* scaleable = dynamic_cast<const dmt::IScaleable*>(getSelf())) {
      // Try to get the scale value from the Scaleable base
      // This assumes the scale member is public and accessible
      // Use reinterpret_cast to get the actual Scaleable<Derived> pointer
      // since IScaleable does not expose scale directly
      // This is safe if getSelf() is a Scaleable<Derived>
      auto* derivedScaleable =
        reinterpret_cast<const dmt::Scaleable<Derived>*>(scaleable);
      scale = derivedScaleable->scale;
    }

    // Multiply the position by the scale
    position = position * scale;

    // Find the host context
    const auto* hostContext = editor->getHostContext();
    if (hostContext == nullptr) {
      jassertfalse; // Could not find the host context
      return;
    }

    // Create the context menu
    std::unique_ptr<juce::HostProvidedContextMenu> contextMenu =
      hostContext->getContextMenuForParameter(target);
    if (contextMenu == nullptr) {
      jassertfalse; // Could not create the context menu
      return;
    }

    // Show the context menu
    contextMenu->showNativeMenu(position.toInt());
  }

private:
  //============================================================================
  /**
   * @brief Helper to get the derived class pointer (CRTP).
   */
  inline Derived* getSelf() noexcept { return static_cast<Derived*>(this); }

  //============================================================================
  /**
   * @brief Iterate the component tree upwards to find the editor.
   *
   * This function is used to find the editor component in the hierarchy.
   * It starts from the current component and moves up the parent chain until
   * it finds a component that is an instance of the editor type.
   */
  template<typename EditorType>
  EditorType* findEditor() noexcept
  {
    auto* currentComponent =
      static_cast<juce::Component*>(static_cast<Derived*>(this));
    while (currentComponent != nullptr) {
      if (auto* editor = dynamic_cast<EditorType*>(currentComponent))
        return editor;

      currentComponent = currentComponent->getParentComponent();
    }
    jassertfalse; // Could not find the editor in the hierarchy
    return nullptr;
  }
};
} // namespace dmt