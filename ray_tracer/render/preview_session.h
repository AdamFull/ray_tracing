#pragma once

class CRayEngine;

// Runs the interactive real-time preview: a Win32 window showing a progressively-refining
// path-traced viewport with fly-camera controls. Pressing P launches a full-quality render
// (using the configured resolution / sample count) and saves it to the configured output.
//
// Controls:
//   Right mouse + drag : look
//   W/A/S/D            : move,  E / Space: up,  Q / Ctrl: down,  Shift: move faster
//   Mouse wheel        : field of view
//   P                  : full-quality render -> output image
//   Esc / close        : quit
void run_preview(CRayEngine* engine);
