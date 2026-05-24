#pragma once

// Maps normalized image coordinates [0,1] (mirror-corrected) to keyboard space
// [0,1] using the user's comfortable reach rectangle.
struct Calibration
{
  float x0 = 0.15f, y0 = 0.15f; // reach rect top-left in image space
  float x1 = 0.85f, y1 = 0.85f; // reach rect bottom-right
  bool valid = false;           // false => using the default rect above

  // Returns keyboard-space (kx,ky) in [0,1]. Values outside the reach rect map
  // outside [0,1] (caller treats those as "no key").
  void toKeyboard(float imageX, float imageY, float &kx, float &ky) const
  {
    float w = (x1 - x0), h = (y1 - y0);
    kx = w != 0.0f ? (imageX - x0) / w : 0.0f;
    ky = h != 0.0f ? (imageY - y0) / h : 0.0f;
  }
};
