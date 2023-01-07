#include <stdio.h>
#include <iostream>
#include <string.h>
#include <json.hpp>
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <emscripten/fetch.h>
#include "graphviz/gvc.h"
#include <SDL.h>
#include <functional>
#include <emscripten.h>
#include "opengl.h"

using json = nlohmann::json;

extern gvplugin_library_t gvplugin_core_LTX_library;
extern gvplugin_library_t gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t gvplugin_neato_layout_LTX_library;

void construct_graph() {
  Agraph_t* G = agmemread("graph {}");
  GVC_t* gvc;
  gvc = gvContext();
  gvAddLibrary(gvc, &gvplugin_core_LTX_library);
  gvAddLibrary(gvc, &gvplugin_dot_layout_LTX_library);
  gvAddLibrary(gvc, &gvplugin_neato_layout_LTX_library);

  gvLayout (gvc, G, "dot");
  char * result;
  unsigned int length = 0;
  gvRenderData (gvc, G, "xdot", &result, &length);

  std::cout << result << std::endl;
  
  gvFreeRenderData(result);
}

void probe_response_data(emscripten_fetch_t *fetch) {
  json j;
  try
  {
      j = json::parse(fetch->data);
  }
  catch (json::parse_error& ex)
  {
      std::cerr << "parse error at byte " << ex.byte << std::endl;
  }
  std::cout << j.dump() << std::endl;
  construct_graph();
}

void downloadSucceeded(emscripten_fetch_t *fetch) {
  printf("Finished downloading %llu bytes from URL %s.\n", fetch->numBytes, fetch->url);

  probe_response_data(fetch);
  // The data is now available at fetch->data[0] through fetch->data[fetch->numBytes-1];
  emscripten_fetch_close(fetch); // Free data associated with the fetch.
}

void downloadFailed(emscripten_fetch_t *fetch) {
  printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
  emscripten_fetch_close(fetch); // Also free data on failure.
}


int main() {
  try {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
    const char* headers[] = {"x-api-key", "J3TEmAU4Prnw96_xtfKkwQ", NULL};
    attr.requestHeaders = headers;


    emscripten_fetch(&attr, "https://dev1.dev-usw2-sematic0.sematic.cloud/api/v1/runs/d0a340b14e04483bbab8ccfedec51ad8/graph?root=1");

    try_opengl();
    
    printf("hello, world!\n");
    return EXIT_SUCCESS;
  } catch( const std::exception &e) {
    
    printf("Caught stuff %s\n", e.what());
  }
}
