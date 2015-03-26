# creates a coarse pbfhr mk1 reactor mesh using vtk
import meshpy

# radius of central reflector [m]
r_cr = 0.5

# number of radial zones in fuel region [m]
n_fz = 4

# thickness of fuel pebble layer [m]
t_fp = 1

# thickness of graphite pebble layer [m]
t_gp = 0.05

# thickness of outer reflector [m]
t_or = 1

# height of active fuel region [m]
h_active = 4

# height of defuelling chute [m]
h_defuel = 0.5

# height of refuelling chute [m]
h_refuel = 0.5

# inner radius of outer reflector [m]
r_or_inner = r_cr + t_fp + t_gp

# outer radius of outer reflector [m]
r_or_outer = r_or_inner + t_or


def setup():


def teardown():


def add_cyl(r, h):


