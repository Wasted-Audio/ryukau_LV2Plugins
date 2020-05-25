# UhhyouPlugins Color
If you made a nice color theme, feel free to send a patch to: https://github.com/ryukau/LV2Plugins .

## File Location
Color configuration is placed at `$XDG_CONFIG_HOME/UhhyouPlugins/style/style.json`. If `$XDG_CONFIG_HOME` is empty, it becomes `$HOME/.config`.

- [XDG Base Directory Specification](https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html)

To change theme, overwrite `style.json` with one of the `theme/*.json`.

`style.json` will be loaded for each time a plugin window opens.

## Colors
Below is a example of `style.json`.

```json
{
  "foreground": "#ffffff",
  "foregroundButtonOn": "#000000",
  "background": "#353d3e",
  "boxBackground": "#000000",
  "border": "#808080",
  "unfocused": "#b8a65c",
  "highlightMain": "#368a94",
  "highlightAccent": "#2c8a58",
  "highlightButton": "#a77842",
  "highlightWarning": "#8742a7",
  "overlay": "#ffffff88",
  "overlayHighlight": "#00ff0033"
}
```

Hex color codes are used.

- 6 digit color is RGB.
- 8 digit color is RGBA.

First letter `#` is conventional. Plugins ignore the first letter of color code, thus `?102938`, ` 11335577` are valid.

Do not use characters outside of `0-9a-f` for color value.

- `foreground`: Text color.
- `foregroundButtonOn`: Text color of active toggle button. Recommend to use the same value of `foreground` or `boxBackground`.
- `background`: Background color.
- `boxBackground`: Background color of inside of box shaped components (Barbox, Button, Checkbox, OptionMenu, TextKnob, VSlider).
- `border`: Border color of box shaped components.
- `unfocused`: Color to fill unfocused components. Currently, only used for knobs.
- `highlightMain`: Color to indicate focus is on a component. Highlight colors are also used for value of slider components (BarBox and VSlider).
- `highlightAccent`: Same as `highlightMain`. Used for cosmetics.
- `highlightButton`: Color to indicate focus is on a button.
- `highlightWarning`: Same as `highlightMain`, but only used for parameters which requires extra caution.
- `overlay`: Overlay color. Used to overlay texts and indicators.
- `overlayHighlight`: Overlay color to highlight current focus.

## Color Config Source Code
This section describes how to modify source code to add/change colors. Source code is available on following link.

- https://github.com/ryukau/LV2Plugins

Following steps are required to change colors in code.

1. Add color and style to palette.
2. Change GUI component code to add extra style.
3. Change plugin specific GUI code to apply style to individual component.

### Adding Color and Style
Colors and styles are defined in `common/gui/style.hpp` and `common/gui/style.cpp`.

Colors are defined in `Palette` class. To add color, follow the steps below.

1. Add color variable to `Palette`.
2. Add method to read color variable to `Palette`.
3. Add `loadColor()` to `Palette::load()` in `common/gui/style.cpp`.

Styles are defined as `Style` enum class in `common/gui/style.hpp`. It is better to have a context for a style, rather than purely cosmetic one.

### Change GUI Component Code
GUI component codes are placed in `common/gui`.

To add style, look into `onNanoDisplay()` method. There are codes like following:

```c++
// `pal` is defined as `Palette &pal;`.
if constexpr (style == Style::accent) {
  strokeColor(isMouseEntered ? pal.highlightAccent() : pal.unfocused());
} else if (style == Style::warning) {
  strokeColor(isMouseEntered ? pal.highlightWarning() : pal.unfocused());
} else {
  strokeColor(isMouseEntered ? pal.highlightMain() : pal.unfocused());
}
```

When you defined new style, add a branch to these style conditions. Searcing under `common/gui` with a query `if constexpr` may help to find style conditions.

### Change Plugin Specific GUI Code
Each plugin has a specific GUI code at `PluginName/ui.cpp`.

For example, `EsPhaser/ui.cpp` has a code like following:

```c++
public:
  EsPhaserUI() : PluginUIBase(defaultWidth, defaultHeight)
  {
    /* ... */

    using ID = ParameterID::ID;

    const auto top0 = 20.0f;
    const auto left0 = 20.0f;

    // Phaser.
    const auto phaserTop = top0 - margin;
    const auto phaserLeft = left0;

    addKnob(phaserLeft, phaserTop, knobWidth, margin, uiTextSize, "Mix", ID::mix);
    addKnob(
      phaserLeft + knobX, phaserTop, knobWidth, margin, uiTextSize, "Freq",
      ID::frequency);
    addKnob(
      phaserLeft + 2.0f * knobX, phaserTop, knobWidth, margin, uiTextSize, "Spread",
      ID::freqSpread);
    addKnob<Style::warning>(
      phaserLeft + 3.0f * knobX, phaserTop, knobWidth, margin, uiTextSize, "Feedback",
      ID::feedback);

    /* Rest of addSomeComponent codes. */
  }
```

Note that `addKnob<Style::warning>`. `addKnob` and other methods to add components has a template argument for a style. If the style template argument is not set, it defaults to `Style::common`.

Now you can apply the style to individual component.

To read implementation of `add*()` methods, see `common/uibase.hpp`.
