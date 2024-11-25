import math

## Moment of Interia Calcs
#-------------------------

# Daedalus 4B
m_plane = 2.0
wingspan = 2.0
width = 0.07

I_plane = 1/12 * m_plane * (wingspan**2 + width**2)

# Daedalus 4A
m_D = 4.0
c = 0.3

I_D = 1/12 * m_D * 2*(0.3**2)

# Rod
m_rod = 0.5
l = 1.0

I_rod = 1/12 * m_rod * l**2

# Total
I = I_plane + I_D + I_rod

# print(f"Moments of inertia: \n\n\tPlane = {I_plane:.2f} \n\tDaedalus 4A = {I_D:.2f} \n\tRod = {I_rod:.2f} \n\n\tTotal = {I:.2f} kgm^2\n")

## Torque Calcs
#-------------------------
slew =  math.pi/180

tau_max = I * slew

print(f"\nTorque = \t {tau_max} Nm")

# print(f"Minimum torque required: {tau_max:.2g} Nm")

## Reaction Wheel Sizing
#-------------------------
rpm = 350 # Max rpm of motor
omega_r = 2*math.pi * rpm / 60

# I_r  = I * slew / omega_r

# 3D printed version with nuts and bolts
# M9
screw_m = 0.006 # kg
nut_m = 0.003 # kg
r_screw = 0.150 / 2 #m
n_screw = 24
n_nut = 4

mass_rw = n_screw*(screw_m + n_nut*nut_m)

I_r = mass_rw*r_screw**2
inertia_ratio = I/I_r
omega_r = I * slew/ I_r
rpm = omega_r/(2*math.pi)*60

print(f"\n3D printed flywheel design:")
print(f"\nIr = \t\t\t {I_r} kgm^2 \nInertia ratio = \t {inertia_ratio} \nRPM = \t\t\t {rpm} RPM\nMass = \t\t\t {mass_rw} kg")

# ---- Find max speed
chosen_RPMmax = 350
omega_max = chosen_RPMmax * I_r / I
print(f"\nSaturation velocity of plane = \t\t\t {omega_max} rad/s")
