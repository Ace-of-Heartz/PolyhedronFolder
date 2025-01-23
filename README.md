# PolyhedronFolder

## Description

This repository contains the source code of a custom *.poly* file editor and viewer.

### .poly files

A custom file extensions which is used for a file containing the procedural description of meshes consisting of regular polygon faces called polyhedrons.

## Implementation Details

Used **APIs**:

- OpenGL3
- SDL2
- ImGui


## Examples

### Example Files

All example assets can be found in [Assets](Assets/ExamplePoly), most of which were made outside the editor.

## To-Do

For the future, the following implementations and changes are planned:

- Refactor / migrate mos of the source code into a Vulkan backend.
- Implement / experiment with different types of face descriptions and draw call orders for more effectiveness
  - Including: Recalculation of normals direction, face updates
- Implement in-world highlighting of hovered faces.
- More information and editing options for polyhedrons
- Loading and handling multiple polyhedrons at once
- Custom builder interface with cubes: Use "cube" to make more, unique polyhedrons
  - Implement this for any polyhedrons, not just cubes
- Intersection handling: In folded state and unfolded state as well
  - Option to toggle indicating and allowing face intersections
- Graph construction from starting tree: Graph indicates the relationship of all faces in a folded state
- Random tree construction from graph: Tree indicates the relationship of all faces in an unfolded state
- Custom attributes for polyhedrons and faces if necessary
  - For example:
    - Weight
- Improve on README + add example media