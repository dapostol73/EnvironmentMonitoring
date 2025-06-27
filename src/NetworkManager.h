#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <WiFi.h>

#include "ApplicationSettings.h"

namespace A12Studios
{
    class NetworkManager
    {
        private:

        public:
            NetworkManager();
            bool init();
            int scanSettingsID(ApplicationSettings* aSettings, uint16_t nSettings);
            bool connectWiFi(WiFiConnection wiFiConnection, uint16_t retryAttempts = 2, uint16_t retryDelay = 20);
            String getLocalIP();
    };
}

#endif