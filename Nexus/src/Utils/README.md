# Utilities  

---

## Contains  

1. **Logger**  
   - Purpose: Provides logging functionality for different levels of messages.  
   - Functions:  
     - `Log()`: Prints text in green.  
     - `Warn()`: Prints text in yellow.  
     - `Err()`: Prints text in red.  
   - Note:  
     - This doesn't work in Visual Studio 2022 as it is a Windows App and not a Console App. However, it works in Rider.

2. **Vector2**  
   - Purpose: A struct representing a 2D vector with two floats (`x`, `y`).  
   - Features:  
     - Includes helper overloads for comparison and basic vector calculations.  
     - Supports operations such as Dot product, Cross product, and normalization.  
     - Provides methods for vector addition, subtraction, scalar multiplication, and magnitude calculation.  

3. **Color**  
   - Purpose: A struct representing RGB color with three floats (`r`, `g`, `b`).  
   - Features:
     - Includes an Enum `Colors` which contains a some popular colors. Usage `Color(Colors::RED)`.

4. **GraphicsUtils**  
   - Purpose: Provide helper functions to draw basic shapes.  
   - Features: Have functions to 
     - Print Text: `Graphics::PrintText()` 
     - Draw lines and outlined shapes:`Graphics::DrawLine()`, `Graphics::DrawCircle()` and `Graphics::DrawPolygon()`
     - Draw filled shapes: `DrawFillCircle()`, `DrawFillPolygon()` and `DrawFillRectangle()`

5. **Math**  
   - Purpose: Provide helper functions to Linear interpolate between two values `start` and `end` w.r.t `t`. If `t=0` means `start` and `t=1` means `end`. 
   - Features: `Lerp` function overloads for `float`, `Vector2` and `Color`

6. **Random**  
   - Purpose: Provide helper functions to generate random values. Usage `Random::Float()`.
   - Features: 
     - `Float()`: Return a random float value between `0` and `1`.
     - `Float(min, max)`: Return a random float value between `min` and `max`.
     - `Int()`: Return a random int value between `min` and `max`.
   - *This RNG helper is inspired by [Cherno](https://www.youtube.com/watch?v=GK0jHlv3e3w)*

7. **VectorN**  
   - Purpose: A struct representing a N-dimensional vector. N represented by `dimensions` and the content represented by `data`.  

8. **Matrix**  
   - Purpose: A struct representing a MxN matrix, using `numRows` and `numCols`.

---
