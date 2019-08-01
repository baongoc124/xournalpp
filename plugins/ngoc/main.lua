var_dump = require "var_dump"

-- default colors from xournalpp
COLORS = {0x000000, 0x008000, 0x00c0ff, 0x00ff00, 0x3333cc, 0x808080, 0xff0000, 0xff00ff, 0xff8000, 0xffff00}
ACCELERATORS = {"grave", "1", "2", "3", "4", "5", "6", "7", "8", "9"}

-- Register all Toolbar actions and intialize all UI stuff
function initUi()
  print("Hello from Ngoc\n");

  for i, c in ipairs(ACCELERATORS) do
    print("Creating menu for color " .. i)
    ref = app.registerUi({["menu"] = "Color " .. i, ["callback"] = "setColor" .. i, ["accelerator"] = c});
    print("Menu reference:");
    var_dump(ref);
  end

  print("Ngoc's plugin registered\n");
end

-- Callback if the menu item is executed
-- copy & paste is faster than finding how to create/assign dynamically callback functions so ...
function setColor1()
  result = app.setToolColor(0x000000)
  var_dump(result)
end

function setColor2()
  result = app.setToolColor(0x008000)
  var_dump(result)
end

function setColor3()
  result = app.setToolColor(0x00c0ff)
  var_dump(result)
end

function setColor4()
  result = app.setToolColor(0x00ff00)
  var_dump(result)
end

function setColor5()
  result = app.setToolColor(0x3333cc)
  var_dump(result)
end

function setColor6()
  result = app.setToolColor(0x808080)
  var_dump(result)
end

function setColor7()
  result = app.setToolColor(0xff0000)
  var_dump(result)
end

function setColor8()
  result = app.setToolColor(0xff00ff)
  var_dump(result)
end

function setColor9()
  result = app.setToolColor(0xff8000)
  var_dump(result)
end

function setColor10()
  result = app.setToolColor(0xffff00)
  var_dump(result)
end
