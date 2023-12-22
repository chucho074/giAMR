# giAMR - Auto Model Redux
This project creates a GUI for [nvdiffmodeling](https://github.com/chucho074/nvdiffmodeling).


## Build 
This project expects an enviroment variable "DEVLIBS", where the folders needs to be
```
include
lib
```

In the "include" folder, expects to have the folder or files that are usually in the folder with the same name of the libs used.

In the "lib" folder, expects to have the .lib for use the libs, this needs to be inside an "x64" folder. 

## Installation
This project follows the installation process of the original [nvdiffmodeling](https://github.com/NVlabs/nvdiffmodeling), the next part is the used installation process.


Requirements:
 - [Microsoft Visual Studio](https://visualstudio.microsoft.com/) 2019+ with Microsoft Visual C++ installed
 - [Cuda](https://developer.nvidia.com/cuda-toolkit) 10.2+
 - [Pytorch](https://pytorch.org/) 1.6+

For building: (with that exact name on DEVLIBS)
 - [pmp](https://github.com/chucho074/pmp-library)
 - [Assimp](https://github.com/assimp/assimp) 5.1+
 - [ImGui](https://github.com/ocornut/imgui)
 - [yaml](https://github.com/jbeder/yaml-cpp)
 - [JSON](https://github.com/nlohmann/json)
 - [stb](https://github.com/nothings/stb)
 - [eigen-3.4.0](https://gitlab.com/libeigen/eigen) 3.4+

Tested in Anaconda3 with Python 3.6 and PyTorch 1.10, CUDA 11.3.
If needed, the version of Anaconda Navigator tested was 2.0.3 [2021.05](https://repo.anaconda.com/archive/). **Note:** Install it on the default path.


## One time setup (Windows)

1. Install [Microsoft Visual Studio](https://visualstudio.microsoft.com/) 2019+ with Microsoft Visual C++. 
2. Install [Cuda](https://developer.nvidia.com/cuda-toolkit) 10.2 or above. **Note:** Install CUDA toolkit from https://developer.nvidia.com/cuda-toolkit (not through anaconda)
3. Install the appropriate version of PyTorch compatible with the installed Cuda toolkit.
Below is the process used in commands:

```
conda create -n dmodel python=3.6
activate dmodel
conda install pytorch==1.10.0 torchvision==0.11.0 torchaudio==0.10.0 cudatoolkit=11.3 -c pytorch -c conda-forge
conda install imageio
pip install PyOpenGL glfw
```

4. Install [nvdiffrast](https://github.com/NVlabs/nvdiffrast) in the `dmodel` conda env. **Note:** The name of the conda env is required to be 'dmodel'. Follow the [installation instructions](https://nvlabs.github.io/nvdiffrast/#windows).


## Usage
The first time the app is open, you will need to set the path for run the scripts, for this you need to set the folder anaconda3/Scripts/ that may be on your user folder.

You need to select an Reference Mesh for continue in the program, the Reference meshes only works if is 1 mesh in the model/file.

The output directory is /Resources/Generated/, there you can find a folder with the name of the model you use, inside the folder you can find a folder with the number of times you try with that model.

If you close the console process of nvdiffmodeling and you can't start again the process, you can check this folder:
```
../AppData/Local/torch_extensions/torch_extensions/Cache/py36_cu113/renderutils_plugin
```
if any "lock" file exists, delete it.
