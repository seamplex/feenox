#!/bin/python

import sys
import pyvista as pv
import imageio
import numpy as np

if len(sys.argv) < 2:
   print("usage: %s results.pvd" % sys.argv[0])
   exit(0)
   
pvd_file = sys.argv[1]             # path to your .pvd file
scale_factor = 1.0                 # adjust deformation scaling
output_file = "%s.mp4" % pvd_file  # output video name
n_frames = 480                     # total frames in animation (smoothing)
fps = 24                           # frames per second

# === Load dataset ===
reader = pv.get_reader(pvd_file)
all_times = reader.time_values

all_meshes = []
for t in all_times:
    reader.set_active_time_value(t)
    mb = reader.read()
    # unwrap first block if MultiBlock
    if isinstance(mb, pv.MultiBlock):
        mesh = mb[0]
    else:
        mesh = mb
    all_meshes.append(mesh)


# Ensure displacement array is named correctly
disp_name = None
for name in all_meshes[0].point_data:
    if all_meshes[0][name].ndim == 2 and all_meshes[0][name].shape[1] in (2, 3):
        disp_name = name
        break
if disp_name is None:
    raise RuntimeError("Could not find a vector displacement array in the file.")

# === Normalize time values to [0,1] ===
t_values = np.array(all_times)
t_min, t_max = t_values[0], t_values[-1]

def interpolate_mesh(t_norm):
    """Return a mesh warped at normalized time t_norm in [0,1], 
    respecting irregular time steps."""
    target_time = t_min + t_norm * (t_max - t_min)

    # find surrounding indices
    i1 = np.searchsorted(t_values, target_time)
    if i1 == 0:
        return all_meshes[0]
    if i1 >= len(t_values):
        return all_meshes[-1]
    i0 = i1 - 1

    # interpolation weight
    t0, t1 = t_values[i0], t_values[i1]
    alpha = (target_time - t0) / (t1 - t0)

    m0, m1 = all_meshes[i0], all_meshes[i1]
    u0 = m0.point_data[disp_name]
    u1 = m1.point_data[disp_name]
    u = (1 - alpha) * u0 + alpha * u1

    warped = m0.copy()
    warped.points = m0.points + scale_factor * u
    return warped


# === Create animation ===
plotter = pv.Plotter(off_screen=True)
writer = imageio.get_writer(output_file, fps=fps, codec='libx264')

plotter.view_xy()
plotter.add_mesh(all_meshes[0], color="lightgray", style="wireframe", opacity=0.2)
actor = plotter.add_mesh(interpolate_mesh(0), scalars=None, color="lightblue")

for frame in range(n_frames):
    print(frame)
    t_norm = frame / (n_frames - 1)
    warped = interpolate_mesh(t_norm)
    actor.mapper.SetInputData(warped)
    plotter.reset_camera()
    img = plotter.screenshot(return_img=True)
    writer.append_data(img)

writer.close()
print(f"Saved animation to {output_file}")
