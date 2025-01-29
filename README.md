# Nexus Game Engine (Ubisoft Next Project)

[YouTube Link](https://youtu.be/AFiTRo31Kq4)

---
A 2D mini-golf game project for the Ubisoft **Ubisoft NEXT 2024-2025 Programming Challenge**!

The game is built on top of an ECS (Entity Component System) game engine. The strength of this engine lies in physics simulation, impulse-based collision resolution and procedurally generated maps. 

Key Features of the Game Engine:
1. Physics System: Simulates forces like drag, friction, springs, and gravity with seamless integration into position and rotation.
2. Collision Resolution: Features an advanced impulse-based constraint system for stable and accurate collision handling.
3. Additional Systems:
Particle Effects for Visual Flair.
Event Management System for smooth interaction handling.
Input Management System for precise player controls.

Minigolf Game Highlights:
1. Procedural Content Generation (PCG): Randomly generates terrain, obstacles, and wind conditions for unique gameplay every time.
2. Customizable Worlds: Play in low, normal, or extreme conditions with varying gravity, wind speed, and drag.
3. Dynamic Gameplay: Choose between weak, normal, or power shots to adapt to changing physics on different planets.

By combining different gravity, wind, drag and random maps with strategic shot options, every map will be a unique experience.
---

## Credits

1. **Dreamy Space Soundtrack**  
   - [Asset Link](https://chiphead64.itch.io/dreamy-space-soundtrack)
   - *For Background sound* 
2. **Background Elements Redux (Kenney Vleugels)**  
   - [Asset Link](https://www.kenney.nl/assets/background-elements-redux)  
3. **Physics Assets (Kenney Vleugels)**  
   - [Asset Link](https://www.kenney.nl/assets/physics-assets)
   - [License](Nexus/Assets/Sprites/kenney_physics-assets/License.txt)   
   - *For stone, glass, wood shapes sprites* 
4. **Royalty Free Sound Effects Archive: GameAudioGDC (Sonniss.com)**  
   - [Asset Link](https://sonniss.com/gameaudiogdc)  
   - [License](Nexus/Assets/Audio/License.pdf)  
5. **Golf Swing (mixkit.co/)**  
   - [Asset Link](https://mixkit.co/free-sound-effects/golf/)  
   - [License](https://mixkit.co/license/#sfxFree)  
6. **Rainbow + White Pixel Flag Animation Bundle**  
   - [Asset Link](https://decrebrian.itch.io/pixel-flag-animation-bundle)  
7. **Sport Balls Pixel-art Pack**  
   - [Asset Link](https://beemaxstudio.itch.io/sport-balls-pixel-pack)
   - *For golf ball*  
8. **Pixel Explosions Sound Effect Pack [SFX]**  
   - [Asset Link](https://jdsherbert.itch.io/pixel-explosions-sfx-pack)
   - *For explosion sound* 
9. **Cursor Pack**  
   - [Asset Link](https://www.kenney.nl/assets/cursor-pack)
   - *For Start arrow* 
  

---

## Learning Resources

### Books

1. **Game Programming Patterns (Robert Nystrom)**  
   - [Link](https://gameprogrammingpatterns.com/)  

2. **Game Engine Architecture, Third Edition (Jason Gregory)**  
   - [Link](https://www.gameenginebook.com/) 

### Online Texts

1. **A Simple Entity Component System (Austin Morlan)**  
   - [Link](https://austinmorlan.com/posts/entity_component_system/)  
   - *My ECS system is heavily inspired by this blog post. To be honest, I struggle with naming things, so most of my ECS class names are drawn directly from this.*  

2. **Studyplan.dev**  
   - [Link](https://www.studyplan.dev/)  
   - *My journey into C++ began here. Most of my basic and intermediate C++ knowledge comes from this site.*  

3. **Nomad Game Engine: Part 7—The Event System (Niko Savas)**  
   - [Medium Link](https://medium.com/@savas/nomad-game-engine-part-7-the-event-system-45a809ccb68f)  
   - *My event system is inspired by this blog post.*  

4. **Architecture of a System for Reading Input Devices Part 1 (Eugene Solyanov, posted by _Engine_)**  
   - [Link](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/architecture-of-system-for-reading-input-devices-part-1-r4805/)  
   - *This article provided useful info while brainstorming the architecture of my input system.*
  
5. **Coding a 2D physics engine from scratch — and using it to simulate a pendulum clock (William Seymour)**  
   - [Link](https://medium.com/@www.seymour/coding-a-2d-physics-engine-from-scratch-and-using-it-to-simulate-a-pendulum-clock-964b4ac2107a)  
   - *In this article, I found the equation to calculate the Impulse imparted by the collision, and subsequently its effect on the linear and angular velocity.*

6. **Equality Constraints (Hubert Eichner)**  
   - [Link](https://web.archive.org/web/20211022100314mp_/http://myselph.de/gamePhysics/equalityConstraints.html)  
   - *This article contains equations and the derivation for the Joint Constraint.*

7. **Video Game Physics Tutorial - Part III: Constrained Rigid Body Simulation (Nilson Souto)**  
   - [Link](https://www.toptal.com/game/video-game-physics-part-iii-constrained-rigid-body-simulation)  
   - *In this article, I found the equation to calculate the Jacobian matrix to calculate correctional impulse.*

8. **Computing tensor of Inertia in 2D (StackOverFlow)**  
   - [Link](https://stackoverflow.com/questions/41592034/computing-tensor-of-inertia-in-2d/41618980#41618980)  
   - *In this article, I found the equation to calculate the Moment of inertia for convex polygons.*

### YouTube

1. **C++ and Game Engine Video Series (The Cherno)**  
   - [Channel Link](https://www.youtube.com/@TheCherno)  
   - *This channel inspired me to dive into game engine development.*  

2. **C++ Game Engine Infrastructure (ChiliTomatoNoodle)**  
   - [Channel Link](https://www.youtube.com/@ChiliTomatoNoodle)  
   - *Helpful for setting up project properties and understanding C++ rulesets.*  

3. **Mike Acton: "Data-Oriented Design and C++"**  
   - [Video Link](https://www.youtube.com/watch?v=rX0ItVEVjHc&list=LL)  
   - *I’ve watched this video multiple times but still feel there’s more to absorb. It was my first step into data-oriented design.*

4. **The Cherno: "Making a PARTICLE SYSTEM in ONE HOUR! (NO ENGINES)"**  
   - [Video Link](https://www.youtube.com/watch?v=GK0jHlv3e3w&t=268s)  
   - *This video was super helpful while I was building the particle effect system.*

5. **The Cherno: "CAMERAS and How They Work | Game Engine series"**  
   - [Video Link](https://www.youtube.com/watch?v=LfbqtmqxX04)  
   - *This video was super helpful in understanding Cameras*    

### Miscellaneous

1. **Unity Game Engine**  
   - *I am somewhat familiar with game development using Unity3D, and I’ve kept that experience in mind while developing this game engine. You can see similarities in the code structure and naming conventions.*  

---
