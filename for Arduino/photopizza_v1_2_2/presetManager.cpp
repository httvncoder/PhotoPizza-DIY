#include "presetManager.h"

using namespace PhotoPizza;

void presetManager::init() {

  _curPreset = 0;
  _curParam = SPEED;
  byte flag;
  EEPROM_readAnything(0, flag);

  /*if (flag == 204) { // 11 00 11 00
    EEPROM_readAnything(1, presets[0]); //  first int in memory
    EEPROM_readAnything(1 + PRESET_SIZE, presets[1]); //
    EEPROM_readAnything(1 + PRESET_SIZE * 2, presets[2]); //
    EEPROM_readAnything(1 + PRESET_SIZE * 3, presets[3]); //
    return;
  }*/ //TODO: restore

  _presets[0]._speed = 4300;
  _presets[0]._steps = 3200;
  _presets[0]._acc = 5000;
  _presets[0]._dir = CW;

  _presets[1]._speed = 3300;
  _presets[1]._steps = 3200;
  _presets[1]._acc = 5000;
  _presets[1]._dir = CW;

  _presets[2]._speed = 1000;
  _presets[2]._steps = 10000;
  _presets[2]._acc = 5000;
  _presets[2]._dir = CW;

  _presets[3]._speed = 2000;
  _presets[3]._steps = 20000;
  _presets[3]._acc = 5000;
  _presets[3]._dir = CW;

  flag = 204;
  EEPROM_writeAnything(0, flag);
  EEPROM_writeAnything(1, _presets[0]); //  first byte in memory
  EEPROM_writeAnything(1 + PRESET_SIZE, _presets[1]); //
  EEPROM_writeAnything(1 + PRESET_SIZE * 2, _presets[2]); //
  EEPROM_writeAnything(1 + PRESET_SIZE * 3, _presets[3]); //

}

void presetManager::save() { // read mem -> check for changes -> write if changed => EEPROM live longer =)
  preset orig;
  EEPROM_readAnything((1 + getPresetNumber() * PRESET_SIZE), orig);

  if ((orig._speed != getPreset()._speed) || (orig._steps != getPreset()._steps) //TODO: refactor this
      || (orig._acc != getPreset()._acc) || (orig._dir != getPreset()._dir)) {
    EEPROM_writeAnything((1 + getPresetNumber() * PRESET_SIZE), getPreset());
    byte f = 204;
    EEPROM_writeAnything(0, f);
  }
}

void presetManager::nextParam(){
  _curParam = (param) ((_curParam + 1) % PARAM_COUNT);
}

void presetManager::prevParam(){
  _curParam = (param) ((_curParam + PARAM_COUNT - 1) % PARAM_COUNT);
}

void presetManager::nextPreset() {
  _curParam = SPEED;
  _curPreset = (_curPreset + 1) % NUM_PROGRAMS;
}

void presetManager::prevPreset() {
  _curParam = SPEED;
  _curPreset = (_curPreset + NUM_PROGRAMS - 1) % NUM_PROGRAMS;
}

int presetManager::getPresetNumber() {
  return _curPreset;
}

preset presetManager::getPreset() {
  return _presets[_curPreset];
}

long presetManager::getValue(param pos) {
  long rez = 0;
  switch (pos) {
  case SPEED:
    rez = _presets[_curPreset]._speed;
    break;
  case STEPS:
    rez = _presets[_curPreset]._steps;
    break;
  case ACC:
    rez = _presets[_curPreset]._acc;
    break;
  case DIR:
    rez = _presets[_curPreset]._dir;
    break;
  }
  return rez;
}

void presetManager::setValue(param pos, long val) {
  switch (pos) {
  case SPEED:
    if (val < SPEED_MIN)
      val = SPEED_MIN;
    if (val > SPEED_MAX)
      val = SPEED_MAX;
    _presets[_curPreset]._speed = val;
    break;

  case STEPS:
    if (val < ROT_MIN)
      val = ROT_MIN;
    if (val > ROT_MAX)
      val = ROT_MAX;
    _presets[_curPreset]._steps = val;
    break;

  case ACC:
    if (val < ACC_MIN)
      val = ACC_MIN;
    if (val > ACC_MAX)
      val = ACC_MAX;
    _presets[_curPreset]._acc = val;
    break;

  case DIR:
    if (val == 0) {
      _presets[_curPreset]._dir = CCW;
    } else {
      _presets[_curPreset]._dir = CW;
    }
    break;
  }

}

void presetManager::valueUp(param pos) {
  long val = 0;
  switch (pos) {
  case SPEED: {
    val = SPEED_STEP;
    long tmp = _presets[_curPreset]._speed + val;
    if ((_presets[_curPreset]._speed > SPEED_MAX) || tmp > SPEED_MAX) {
      _presets[_curPreset]._speed = SPEED_MAX;
      break;
    }
    _presets[_curPreset]._speed = tmp;
  }
    break;

  case STEPS: {
    val = ROT_STEP;
    long tmp = _presets[_curPreset]._steps + val;
    if ((_presets[_curPreset]._steps > ROT_MAX) || tmp > ROT_MAX) {
      _presets[_curPreset]._steps = ROT_MAX;
      break;
    }
    _presets[_curPreset]._steps = tmp;
  }
    break;

  case ACC: {
    val = ACC_STEP;
    long tmp = _presets[_curPreset]._acc + val;
    if ((_presets[_curPreset]._acc > ACC_MAX) || tmp > ACC_MAX) {
      _presets[_curPreset]._acc = ACC_MAX;
      break;
    }
    _presets[_curPreset]._acc = tmp;
  }
    break;

  case DIR: {
    val = CW;
    _presets[_curPreset]._dir = CW;
  }
    break;
  }
}

void presetManager::valueDown(param pos) {
  long val = 0;
  switch (pos) {
  case SPEED: {
    val = SPEED_STEP;
    long tmp = _presets[_curPreset]._speed - val;
    if ((_presets[_curPreset]._speed < SPEED_MIN) || tmp < SPEED_MIN) {
      _presets[_curPreset]._speed = SPEED_MIN;
      break;
    }
    _presets[_curPreset]._speed = tmp;
  }
    break;

  case STEPS: {
    val = ROT_STEP;
    long tmp = _presets[_curPreset]._steps - val;
    if ((_presets[_curPreset]._steps < ROT_MIN) || tmp < ROT_MIN) {
      _presets[_curPreset]._steps = ROT_MIN;
      break;
    }
    _presets[_curPreset]._steps = tmp;
  }
    break;

  case ACC: {
    val = ACC_STEP;
    long tmp = _presets[_curPreset]._acc - val;
    if ((_presets[_curPreset]._acc < ACC_MIN) || tmp < ACC_MIN) {
      _presets[_curPreset]._acc = ACC_MIN;
      break;
    }
    _presets[_curPreset]._acc = tmp;
  }
    break;

  case DIR: {
    val = CCW;
    _presets[_curPreset]._dir = CCW;
  }
    break;
  }
}

void presetManager::changeDirection(int dir) {
  _presets[_curPreset]._dir = dir;
}

