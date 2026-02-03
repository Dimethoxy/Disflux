//==============================================================================
#pragma once
//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace gui {
namespace widget {

//==============================================================================

class TextEditor : public juce::TextEditor
{
  using String = juce::String;

public:
  TextEditor(const String _name)
    : juce::TextEditor(_name)
  {
    setMultiLine(false);
    setFont(juce::Font(16.0f));
    setJustification(juce::Justification::centredLeft);
    setEscapeAndReturnKeysConsumed(true);
    setWantsKeyboardFocus(true);
  }

  ~TextEditor() override = default;

  // Callbacks for arrow up/down
  std::function<void()> onArrowUp;
  std::function<void()> onArrowDown;

  bool keyPressed(const juce::KeyPress& key) override
  {
    // Handle the backspace key
    if (key == juce::KeyPress::backspaceKey) {
      if (getText().isNotEmpty()) {
        // Remove the character in front of the caret
        const auto caretPos = getCaretPosition();
        if (caretPos > 0) {
          newTransaction();
          const auto text = getText();
          // split the text into two parts
          const auto firstPart = text.substring(0, caretPos - 1);
          const auto secondPart = text.substring(caretPos);
          // set the new text
          setText(firstPart + secondPart);
          // set the caret position
          setCaretPosition(caretPos - 1);
        }
      }
      return true;
    }

    // Handle arrow up/down
    if (key == juce::KeyPress::upKey) {
      if (onArrowUp)
        onArrowUp();
      return true;
    }
    if (key == juce::KeyPress::downKey || key == juce::KeyPress::tabKey) {
      if (onArrowDown)
        onArrowDown();
      return true;
    }

    // Look for printable characters
    const auto keyCode = key.getKeyCode();
    const bool isLetter =
      (keyCode >= 'a' && keyCode <= 'z') || (keyCode >= 'A' && keyCode <= 'Z');
    const bool isDigit = (keyCode >= '0' && keyCode <= '9');
    const bool isSpecialChar =
      (keyCode == '.' || keyCode == '-' || keyCode == '_' || keyCode == '@' ||
       keyCode == '#' || keyCode == '$' || keyCode == '%' || keyCode == '&' ||
       keyCode == '*' || keyCode == '+' || keyCode == '=' || keyCode == '/' ||
       keyCode == '?' || keyCode == '!' || keyCode == '~' || keyCode == '^' ||
       keyCode == '(' || keyCode == ')' || keyCode == '{' || keyCode == '}' ||
       keyCode == '[' || keyCode == ']' || keyCode == ';' || keyCode == ':' ||
       keyCode == '"' || keyCode == '\'' || keyCode == '<' || keyCode == '>' ||
       keyCode == ',' || keyCode == '|' || keyCode == '`' || keyCode == '\\');
    if (isLetter || isDigit || isSpecialChar) {
      newTransaction();
      auto text = getText();
      auto character = key.getTextCharacter();
      auto string = String::charToString(character);
      insertTextAtCaret(string);
      return true;
    }

    return juce::TextEditor::keyPressed(key);
  }

private:
  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditor)
};

} // namespace widget
} // namespace gui
} // namespace dmt
