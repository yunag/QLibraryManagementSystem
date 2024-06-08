#include "controller.h"

#include "resourcemanager.h"

Controller::Controller() : m_manager(ResourceManager::networkManager()) {}

void Controller::setRestManager(RestApiManager *restManager) {
  m_manager = restManager;
}
