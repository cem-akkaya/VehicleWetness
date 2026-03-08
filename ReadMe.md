

# Vehicle Wetness

 <img src="Resources/Demo1.gif" alt="plugin-vehicle-wetness" width="100%"/>

## Overview

Vehicle Wetness is an Unreal Engine plugin that simulates dynamic rain droplets and wetness masks for vehicles (or any other surfaces) using GPU compute shaders.

This is a foundation-level compute shader only that can lay down the groundwork for a full-fledged solution for multiple windshields or wetness surfaces on vehicles. Currently, it does not perform additional work around mesh or UV processing, or vehicle orientation solving. However, it offers realistic physics behavior with respect to average performance targets and high-fidelity realism, complemented by multiple designer-driven properties.

The component generates a high-performance wetness mask that can be used in your materials to drive effects like rain streaks, pooling, and wiper clearing. It accounts for vehicle velocity, gravity, and even includes a fully functional windshield wiper simulation that pushes and clears droplets in real-time.

---

## Features

- **GPU Accelerated**: Uses Compute Shaders (RDG) for efficient simulation of thousands of droplets.
- **Dynamic Movement**: Droplets move based on surface gravity and vehicle velocity (airflow).
- **Windshield Wiper Simulation**: Real-time interaction where wipers push, clear, and accumulate droplets.
- **Customizable Droplets**: Full control over spawn rates, lifetime, speed, size variation, and jitter.
- **Trail & Decay**: Moving droplets leave behind trails that fade over time.
- **Blur Support**: Integrated blur pass to soften the wetness mask.
- **Blueprint Friendly**: Easily update wiper angles and parameters from Blueprints or Timelines.

---

## Quick Start

1. Add a **VehicleWetness** component to your Actor (e.g., a Vehicle Pawn).
2. Assign a **Render Target** to the `RainRenderTarget` property in the Details panel.
3. Use this Render Target in your vehicle's material (typically as a mask for Roughness, Metallic, or Normal offsets).
4. (Optional) To use wipers, call **Update Wiper Angle** from your Blueprint's Tick or a Timeline.


---

## Component Parameters

### Wetness Settings

**Spawn Rate**  
Number of new droplets spawned per second.

**Max Droplet Life**  
Maximum lifetime of a droplet in seconds before it disappears.

**Average Droplet Speed & Randomness**  
Base speed of droplets in UV units and how much they vary.

**Direction Jitter**  
Random direction variation applied to droplets to prevent perfectly straight lines.

**Droplet Size (Start/End/Random)**  
Controls the scale of droplets as they age.

**Trail Decay**  
Rate at which the trail behind moving droplets fades (1.0 = permanent).

**Velocity Influence (X/Y)**  
How much the vehicle's movement affects droplet direction (simulating airflow).

---

### Wiper Parameters

**Wiper Pivot & Radius**  
Defines the rotation point and length of the wiper in UV space.

**Wiper Thickness**  
How close droplets need to be to the wiper blade to be affected.

**Wiper Centrifugal Strength**  
How much droplets are pushed outward along the blade during a sweep.

**Wiper Stickiness**  
Resistance to being pushed (0.0 = slides easily, 1.0 = resistant).

---

## Resources

<p align="center">
  <img src="Resources/Banner.svg" alt="Vehicle Wetness Banner" width="800">
  <br>
  <i>GitHub Social Preview / Banner (1280x640)</i>
</p>

---

## Technical Details

- **RDG Implementation**: The plugin utilizes the Render Graph system for optimal GPU performance.
- **Compute Shader**: All logic (spawn, move, wiper, blur) runs on the GPU.
- **Texture Resolution**: Supports resolutions from 128 to 4096 (default 1024).

---

## License

This plugin is under the [MIT License](LICENSE.md).

MIT allows commercial use.  
You may use, modify, and distribute the software in commercial products without restriction, provided the original copyright notice is included.

---

## Support Me

If you like the plugin, you can support my work here:

<a href="https://www.buymeacoffee.com/akkayaceq" target="_blank">
<img src="https://cdn.buymeacoffee.com/buttons/default-yellow.png" alt="Buy Me A Coffee" height="41" width="174">
</a>
