# Lab: First Run of Gadget-2

[Back to home](https://github.com/ChenYangyao/N-Body-Course) | [View Jupyter](./wdir/galaxy_collide.ipynb) | [View Gif animation for Galaxy collision](./wdir/galaxy_collide/t2/anim1.avi)

## Introduction

[Gadget-2](https://wwwmpa.mpa-garching.mpg.de/galform/gadget/) is a N-body Tree (optional, PM)/SPH simulation software for astrophysical simulations, e.g. cosmology density field, galaxy evolution and interaction.

Download can be found at the Gadget-2 [website](https://wwwmpa.mpa-garching.mpg.de/galform/gadget/gadget2.tar.gz), in the compressed files you can also find the user manual and the Gadget-2 paper. They are valuable sources of studying Gadget-2.


## An Example Simulation of Two Galaxy Collision

The Gadget-2 source code also provide some examples. In this Lab we try the Galaxy Collision example where two encounter galaxies collide with each other, where 4000 dark matter particles and 2000 star particles are treated collisionlessly. I try this example using 8 cores in a local PC and the running takes about 10 minuts. 

Due to strong gravity and tidal force, the particles are expected to be accelerated and scatterted. Figure 1 shows the dark matter density field, star particles with local density, and velocity of star particles. The animation is made by ParaView.

<table><tr>
    <td><img src="./wdir/galaxy_collide/t2/anim1.gif"></td></tr><tr>
    <td><em>Animated Figure 1: Simulate the galaxy collision with Gadget-2, using the example parameter file in the install package.</em><br> 
    Left panel: star particles, color coded by the local stellar mass density. Right panel: velocities of star particles. At each panel the background purple shading is color-coded by the local dark matter density. [ Download the <a href="./wdir/galaxy_collide/t2/anim1.gif">GIF</a> or <a href="./wdir/galaxy_collide/t2/anim1.avi">AVI</a> animation ]
    </td>
</tr></table>

