#ifndef FAAF_H
#define FAAF_H

using faafCallback = void (*)(unsigned int, unsigned int);

class Faaf {
  private:
    Serial serial;
    faafCallback onCoordinates;

  public:
    void Faaf(Serial serial, faafCallback onCoordinates);
    void setup();
    void loop();
};

#endif
