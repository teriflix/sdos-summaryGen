# summaryGen

C++ library that can be used to generate scene, story and character summary from [Fountain](https://fountain.io) files.

This library will eventually be used by the Scrite project to let users generate scene, story and character summaries while importing scripts into Scrite, and also on demand when triggered by user action within the UI.

A brief description of the screenplay format can be found here: https://jotterpad.app/how-to-format-a-screenplay/

The fountain syntax is described here: https://fountain.io/syntax/

## Expectations

### Must haves
- The C++ library must offer a function to report story summary
- It should offer a function to accept a scene index, and return scene summary for the same.
- It should offer a function to accept a character name, and return a character summary

## Sample files

For now make use of the screenplay for "Big Fish" provied in here: https://fountain.io/_downloads/Big-Fish.fountain
