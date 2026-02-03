//==============================================================================
/*
 * ██████  ██ ███    ███ ███████ ████████ ██   ██  ██████  ██   ██ ██    ██
 * ██   ██ ██ ████  ████ ██         ██    ██   ██ ██    ██  ██ ██   ██  ██
 * ██   ██ ██ ██ ████ ██ █████      ██    ███████ ██    ██   ███     ████
 * ██   ██ ██ ██  ██  ██ ██         ██    ██   ██ ██    ██  ██ ██     ██
 * ██████  ██ ██      ██ ███████    ██    ██   ██  ██████  ██   ██    ██
 *
 * Copyright (C) 2024 Dimethoxy Audio (https://dimethoxy.com)
 *
 * This file is part of the Dimethoxy Library, a collection of essential
 * classes used across various Dimethoxy projects.
 * These files are primarily designed for internal use within our repositories.
 *
 * License:
 * This code is licensed under the GPLv3 license. You are permitted to use and
 * modify this code under the terms of this license.
 * You must adhere GPLv3 license for any project using this code or parts of it.
 * Your are not allowed to use this code in any closed-source project.
 *
 * Description:
 * Networking utilities for version checking and update logic.
 *
 * Authors:
 * Lunix-420 (Primary Author)
 */
//==============================================================================

#pragma once

//==============================================================================

#include <JuceHeader.h>

//==============================================================================

namespace dmt {
namespace version {

//==============================================================================
/**
 * @brief Provides networking utilities for version and update logic.
 *
 * @details
 * This class contains static methods for creating URLs and sending GET requests
 * to the server.
 */
class Networking
{
  //==============================================================================
  // Aliases for convenience
  using String = juce::String;
  using URL = juce::URL;

  //==============================================================================
  // Constants
  static constexpr auto SERVER = "https://api.dimethoxy.com/";

public:
  //==============================================================================
  /**
   * @brief Constructs a URL for the given API endpoint.
   *
   * @param _apiEndpoint The API endpoint to append to the server URL.
   * @return A URL object representing the full request URL.
   *
   * @details
   * This function is used to build the full URL for API requests.
   */
  static inline URL createURL(const String _apiEndpoint) noexcept
  {
    return URL(SERVER + _apiEndpoint);
  }

  //==============================================================================
  /**
   * @brief Sends a synchronous GET request to the specified API endpoint.
   *
   * @param _apiEndpoint The API endpoint to request.
   * @return The response as a String. Returns an empty string on failure.
   *
   * @details
   * This function logs the request and response for debugging. It is intended
   * for use in version and update logic, and is marked inline for real-time
   * safety. If the response is empty, an error is logged and an empty string
   * is returned.
   */
  static inline String sendRequest(const String _apiEndpoint) noexcept
  {
    auto url = createURL(_apiEndpoint);

    std::cout << "Sending request to: " << url.toString(true) << std::endl;

    auto responseString = url.readEntireTextStream(false);

    if (responseString.isEmpty()) {
      std::cerr << "Failed to fetch data from: " << url.toString(true)
                << std::endl;
      return String();
    }

    std::cout << "Response: " << responseString << std::endl;

    return responseString;
  }

  //==============================================================================

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Networking)
};

} // namespace version
} // namespace dmt