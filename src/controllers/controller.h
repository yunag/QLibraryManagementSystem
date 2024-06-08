#ifndef CONTROLLER_H
#define CONTROLLER_H

class RestApiManager;

class Controller {
public:
  Controller();
  virtual ~Controller() = default;

  void setRestManager(RestApiManager *restManager);

protected:
  RestApiManager *m_manager;
};

#endif /* !CONTROLLER_H */
