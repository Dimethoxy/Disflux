// #pragma once
// //==============================================================================
// #include <JuceHeader.h>
// #include <toml++/toml.hpp>
// //==============================================================================
// namespace dmt {
// namespace theme {
// using StyleNode = juce::HashMap<juce::String, juce::String>;
// using StyleTree = juce::HashMap<juce::String, StyleNode>;
// class StyleSheet
// {
// public:
//   //============================================================================
//   StyleSheet(const juce::String& path)
//   {
//     toml::table file;
//     try {
//       file = toml::parse_file(path.toStdString());
//       for (auto [section, content] : file) {
//         auto sectionStringView = section.str();
//         auto sectionString =
//           String(sectionStringView.data(), sectionStringView.size());
//         if (content.is_table()) {
//           StyleNode node;
//           for (auto [key, value] : *content.as_table()) {
//             auto keyStringView = key.str();
//             auto keyString = String(keyStringView.data(),
//             keyStringView.size()); auto valueString =
//             String(value.as_string()->get()); node.set(keyString,
//             valueString);
//           }
//           tree.set(sectionString, node);
//         }
//       }
//     } catch (const toml::parse_error& err) {
//       jassertfalse;
//     }
//   }
//   //============================================================================
//   void applyColour(juce::Colour& target,
//                    const juce::String& nodeName,
//                    const juce::String& key)
//   {
//     if (tree.contains(nodeName)) {
//       auto node = tree[nodeName];
//       if (node.contains(key)) {
//         target = juce::Colour::fromString(node[key]);
//       }
//     }
//   }
//   //============================================================================
//   void applyString(juce::String& target,
//                    const juce::String& nodeName,
//                    const juce::String& key)
//   {
//     if (tree.contains(nodeName)) {
//       auto node = tree[nodeName];
//       if (node.contains(key)) {
//         target = node[key];
//       }
//     }
//   }
//   //============================================================================
//   void applyBool(bool& target,
//                  const juce::String& nodeName,
//                  const juce::String& key)
//   {
//     if (tree.contains(nodeName)) {
//       auto node = tree[nodeName];
//       if (node.contains(key)) {
//         target = node[key] == "true";
//       }
//     }
//   }
//   //============================================================================
//   void applyInt(int& target,
//                 const juce::String& nodeName,
//                 const juce::String& key)
//   {
//     if (tree.contains(nodeName)) {
//       auto node = tree[nodeName];
//       if (node.contains(key)) {
//         target = node[key].getIntValue();
//       }
//     }
//   }
//   //============================================================================
//   void applyFloat(float& target,
//                   const juce::String& nodeName,
//                   const juce::String& key)
//   {
//     if (tree.contains(nodeName)) {
//       auto node = tree[nodeName];
//       if (node.contains(key)) {
//         target = node[key].getFloatValue();
//       }
//     }
//   }

// private:
//   StyleTree tree;
// };
// } // namespace theme
// } // namespace dmt