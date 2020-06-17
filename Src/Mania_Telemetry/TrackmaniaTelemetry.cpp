#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "maniaplanet_telemetry.h"
#include <pybind11/pybind11.h>

class TrackmaniaTelemetry {

    HANDLE hMapFile = NULL;
    void* pBufView = NULL;
    const volatile NManiaPlanet::STelemetry* Shared = NULL;
    NManiaPlanet::Nat32 UpdateNumber = 0;
    NManiaPlanet::STelemetry S;


    public:
        TrackmaniaTelemetry() {
            if (Shared == NULL)
            {
                if (hMapFile == NULL)
                    hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, TEXT("ManiaPlanet_Telemetry"));

                if (hMapFile != NULL)
                {
                    if (pBufView == NULL)
                        pBufView = (void*)MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 4096);
                }

                // Show the wait status in the console title
                static bool show_waiting_title = true;
                if (show_waiting_title && pBufView == NULL)
                {
                    show_waiting_title = false;
                }

                Shared = (const NManiaPlanet::STelemetry*)pBufView;
            }
        }

        struct VehicleInfo {
            float InputSteer;
            float InputGasPedal;
            int InputIsBraking;
            unsigned int SpeedMeter;
        };

        bool update() {
            NManiaPlanet::Nat32 Before = Shared->UpdateNumber;
            memcpy(&S, (const NManiaPlanet::STelemetry*)Shared, sizeof(S));
            NManiaPlanet::Nat32 After = Shared->UpdateNumber;

            return Before == After;
        }

        VehicleInfo get() {
            update();
            VehicleInfo x;
            x.InputGasPedal = S.Vehicle.InputGasPedal;
            x.InputIsBraking = S.Vehicle.InputIsBraking;
            x.InputSteer = S.Vehicle.InputSteer;
            x.SpeedMeter = S.Vehicle.SpeedMeter;
            return x;
        }


        
        ~TrackmaniaTelemetry() {
            if (pBufView)
                UnmapViewOfFile(pBufView);
            if (hMapFile)
                CloseHandle(hMapFile);
        }
};

namespace py = pybind11;

PYBIND11_MODULE(Mania_Telemetry, mod) {
    // optional module docstring
    mod.doc() = "Trackmania Telemetery plugin";

    // bindings to Pet class
    py::class_<TrackmaniaTelemetry>(mod, "TrackmaniaTelemetry")
        .def(py::init())
        .def("update", &TrackmaniaTelemetry::update)
        .def("get", &TrackmaniaTelemetry::get);

    py::class_<TrackmaniaTelemetry::VehicleInfo>(mod, "VehicleInfo")
        .def(py::init())
        .def_readonly("InputSteer", &TrackmaniaTelemetry::VehicleInfo::InputSteer)
        .def_readonly("InputGasPedal", &TrackmaniaTelemetry::VehicleInfo::InputGasPedal)
        .def_readonly("InputIsBraking", &TrackmaniaTelemetry::VehicleInfo::InputIsBraking)
        .def_readonly("SpeedMeter", &TrackmaniaTelemetry::VehicleInfo::SpeedMeter);

}