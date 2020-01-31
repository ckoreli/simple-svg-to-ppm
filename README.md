# Simple SVG to PPM

## File Format
The program supports svg images in the following format:
```
<svg width="250" height="250" xmlns="http://www.w3.org/2000/svg">
  <line x1="50" y1="50" x2="200" y2="200" stroke="black" />
  <line x1="200" y1="200" x2="50" y2="200" stroke="red" />
  <line x1="50" y1="200" x2="200" y2="50" stroke="green" />
  <line x1="200" y1="50" x2="50" y2="50" stroke="blue" />
  ...
</svg>
```
Lines starting with `<line` contain the start (`x1`, `y1`) and end (`x2`,`y2`) coordinates of a line and its color (`stroke=`). The color may either be a six-digit hexadecimal string starting with # (e.g., `#ccff00`) or a color name (`black`, `red`, `green`, `blue`, `cyan`, `yellow` shall be supported). If the color has any other value, the line shall not be drawn and a warning shall be generated.

## Command-Line Interface
The program shall be called as follows:
```sh
rendersvg.exe [-s SIZE] [-b COLOR] SVG_FILE PPM_FILE
```
* `-s` is an optional command-line parameter that instructs the program to scale the output image to the size given in the `SIZE` argument in the form of NxM (e.g., 200x400). If this parameter is given, the program shall calculate a scaling factor for the x-axis and y-axis. The resulting PPM image shall then be NxM pixels large.
* `-b` is an optional command-line parameter that sets the background color of the image. The same color choices as for the line color shall be allowed as the argument `COLOR`. If it is not provided, the background color shall be white.
* `SVG_FILE` is a required command-line parameter that specifies the name of the SVG file to read.
* `PPM_FILE` is a required command-line parameter that specifies the name of the PPM file to create.

## Example Executions
Default scaling, default background:
```sh
render.exe in.SVG out.PPM
```
Scaling to 200 by 100, default background:
```sh
render.exe -s 200x100 in.SVG out.PPM
```
Default scaling, yellow background:
```sh
render.exe -b yellow in.SVG out.PPM
```
Scaling to 200 by 100, red background:
```sh
render.exe -s 200x100 -b red in.SVG out.PPM
```
(Same effect):
```sh
render.exe -s 200x100 -b '#ff0000' in.SVG out.PPM
```
