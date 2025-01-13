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

2. **GraphicsUtils**  
   - Purpose: Provide helper functions to draw basic shapes.  
   - Features: Have functions to 
     - Print Text: `Graphics::PrintText()` 
     - Draw lines and outlined shapes:`Graphics::DrawLine()`, `Graphics::DrawCircle()` and `Graphics::DrawPolygon()`
     - Draw filled shapes: `DrawFillCircle()`, `DrawFillPolygon()` and `DrawFillRectangle()`

---
