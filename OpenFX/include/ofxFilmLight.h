#ifndef _ofxFilmLight_h_
#define _ofxFilmLight_h_

#include "ofxCore.h"

#ifdef __cplusplus
extern "C" {
#endif

#define kOfxFilmLightSuite "OfxFilmLightSuite"

    /*
      Many configurations are possible at runtime in Baselight/Daylight depending on hardware (number of computers, number and type of GPUs, presence of SDI output hardware etc) and setup options.

      Typical process role configurations will be

      
      On a single-computer system (e.g. Baselight ONE, macOS)

      Process | GPU 1
      --------+-------------
      ws*     | all

      Process | GPU 1
      --------+-------------
      ws      | part for UI
      node    | remainder

      Process | GPU 1       | GPU 2
      --------+-------------+------
      ws      | part for UI |
      node    | remainder   |
      offload |             | all
      
      Process | GPU 1       | GPU 2
      --------+-------------+------
      ws      | part for UI |
      offload | remainder   |
      offload |             | all
      

      On the UI host of a two-computer system (e.g. Baselight TWO, Baselight X)

      Process | GPU 1
      --------+------
      host    | all

      
      On the node of a two-computer system (e.g. Baselight TWO, Baselight X)

      Process | GPU 1
      --------+-------
      node    | all

      Process | GPU 1 | GPU 2
      --------+-------+------
      node    | all   |
      offload |       | all
      
      Process | GPU 1 | GPU 2
      --------+-------+------
      node    |       |
      offload | all   | 
      offload |       | all

      Process | GPU 1 | GPU 2 | GPU 3...
      --------+-------+-------+---------
      node    |       |       |
      offload | all   |       |
      offload |       | all   |
      offload |       |       | all
      ...
      
      
      As can be seen above, currently each application process only ever uses one GPU.
      
      (For the avoidance of doubt, the GPU numbering shown above is arbitrary and not reflected in this suite's APIs)

      The host/ws process runs the UI of the application which will only render thumbnails using OFX - except in the ws* case above where the ws process does both UI and rendering (e.g. this is seen on single-GPU macOS systems with no SDI hardware).

      An OFX plugin using the GPU APIs (OpenGL/Cuda/Metal) is required to only use the GPU specified (indirectly) in those API. If the APIs are used as specified, this should work correctly without any code changes.

      An OFX plugin using the CPU API to transfer images to and from the OFX host, but which wishes to make use of GPU resources internally, is (at present) permitted to use any of the GPUs that are visible to the process in which it is run.

      Subject to GPU VRAM availability, the OpenGL API is available on all systems, the CUDA API is available on all Linux systems and the Metal API is available on all macOS systems. Future configurations may support the UI host and node of a Baselight TWO/X system being different architectures (e.g. macOS host and Linux node).


      In the APIs below, the following terminology is used

      "system" - the Baselight/Daylight hardware, comprising one or two nodes
      "node" - a single computer
      "node name" - an arbitrary string naming a node
      "gpu name" - an arbitarary string naming a single GPU
      "process role" - the process type listed above: kFilmLightProcessRoleWS, kFilmLightProcessRoleHost, kFilmLightProcessRoleNode or kFilmLightProcessRoleOffload
    */
      

#define kFilmLightProcessRoleWS        "ws"
#define kFilmLightProcessRoleHost      "host"
#define kFilmLightProcessRoleNode      "node"
#define kFilmLightProcessRoleOffload   "offload"

#define kFilmLightAllNodeNames         "FilmLightAllNodeNames"
#define kFilmLightThisNodeName         "FilmLightThisNodeName"
#define kFilmLightThisProcessRole      "FilmLightThisProcessRole"
#define kFilmLightThisProcessRenderGPU "FilmLightThisProcessRenderGPU"
#define kFilmLightAllRoles             "FilmLightAllRoles"
#define kFilmLightAllRenderGPUs        "FilmLightAllRenderGPUs"
#define kFilmLightCudaDeviceID         "FilmLightCudaDeviceID"
#define kFilmLightCudaDeviceUUID       "FilmLightCudaDeviceUUID"
#define kFilmLightTotalVRAM            "FilmLightTotalVRAM"
#define kFilmLightMetalDevice          "FilmLightMetalDevice"
#define kFilmLightMetalDeviceIndex     "FilmLightMetalDeviceIndex"
    
typedef struct OfxFilmLightSuiteV1 {

    /*
     * filmlightGetSystemInfo()
     *
     * Returns a property set with information about the Baselight/Daylight system on which this plugin is running.
     *
     * properties - if non-NULL on output contains:
     *
     * kFilmLightAllNodeNames - string[n], the names of all the nodes in this system (in no particular order)
     *
     * kFilmLightThisNodeName - string[1], the name of the node from which this call is made (note this might not match "uk.ltd.filmlight.nodename" in host properties, please use only the names returned by this call within this suite)
     *
     * kFilmLightThisProcessRole - string[1], the role of the process from which this call is made
     *
     * kFilmLightThisProcessRenderGPU - string[1], the name of the rendering GPU used by the process from which this call is made. This is therefore the only GPU that plugins in this process can use when for rendering via the OFX GPU APIs
     *
     * The property set MUST be released after use by calling filmlightReleasePropertySet
     */
    OfxStatus (*filmlightGetSystemInfo)(OfxPropertySetHandle * properties);
    
    /*
     * filmlightGetNodeInfo()
     *
     * Returns a property set with information about one particular node of the Baselight/Daylight system on which this plugin is running.
     *
     * nodeName - a name as returned by filmlightGetSystemInfo() in kFilmLightThisNodeName or kFilmLightAllNodeNames
     *
     * properties - if non-NULL, on output contains:
     *
     * kFilmLightAllRoles - string[n], the process roles on the specified node
     *
     * kFilmLightAllRenderGPUs - string[n], the names of all the rendering GPUs in the specified node which will be used for rendering by Baselight/Daylight
     *
     * The property set MUST be released after use by calling filmlightReleasePropertySet
     */
    OfxStatus (*filmlightGetNodeInfo)(const char * nodeName,
				      OfxPropertySetHandle * properties);

    /*
     * filmlightGetGPUInfo()
     *
     * Returns a property set with information about one GPU, which must be visible to the process making this call.
     *
     * gpuName - a GPU name as returned by filmlightGetSystemInfo() in kFilmLightThisProcessRenderGPU or by filmlightGetNodeInfo() in kFilmLightAllRenderGPUs
     *
     * properties - if non-NULL, on output contains:
     *
     * kFilmLightCudaDeviceID - int[1], the CUDA device ID of this GPU, from the perspective of the current process. IMPORTANT: CUDA device IDs are not portable between processes, this ID is only valid in this process [only present on Linux]
     *
     * kFilmLightCudaDeviceUUID - string[1], the CUDA device UUID of this GPU. This is portable between processes [only present on Linux]
     *
     * kFilmLightTotalVRAM - double[1], the total VRAM capacity of this GPU in bytes (note on modern macOS systems this is fabricated, due to the integrated memory architecture of Apple Silicon). IMPORTANT: this is for information, plugins cannot make any assumption about this amount of VRAM they will be permitted to use on any GPU
     *
     * kFilmLightMetalDevice - pointer[1], the id<MTLDevice> of the Metal device of this GPU, in the current process, cast to void* [only present on macOS]
     *
     * kFilmLightMetalDeviceIndex - int[1], the index in the array returned by MTLCopyAllDevices() of the Metal device of this GPU, in the current process, cast to void* [only present on macOS]
     *
     * The property set MUST be released after use by calling filmlightReleasePropertySet
     */
    OfxStatus (*filmlightGetGPUInfo)(const char * gpuName,
				     OfxPropertySetHandle * properties);

 
    /*
     * filmlightReleasePropertySet()
     *
     * Destroys a property set returned by one of the above APIs.
     *
     * properties - a pointer to the property set to be destroyed
     */
    OfxStatus (*filmlightReleasePropertySet)(OfxPropertySetHandle * properties);
    
} OfxFilmLightSuiteV1;

#ifdef __cplusplus
}
#endif

#endif
