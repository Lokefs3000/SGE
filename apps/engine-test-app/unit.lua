local function testPrint(name, result)
    print("test: " .. '"' .. name .. '"' .. " resulted in: " .. tostring(result))
end

local function test_window()
    sge.window.setTitle("Unit")
    testPrint("sge.window.setTitle", sge.window.getRealTitle() == "Unit")
    testPrint("sge.window.getRealTitle", sge.window.getRealTitle() == "Unit")
    testPrint("sge.window.getTitle", sge.window.getTitle() == "Unit")

    sge.window.setWidth(500)
    testPrint("sge.window.getRealWidth", sge.window.getRealWidth() == 500)
    testPrint("sge.window.getRealWidth", sge.window.getRealWidth() == 500)
    testPrint("sge.window.getWidth", sge.window.getWidth() == 500)

    sge.window.setHeight(500)
    testPrint("sge.window.getRealHeight", sge.window.getRealHeight() == 500)
    testPrint("sge.window.getRealHeight", sge.window.getRealHeight() == 500)
    testPrint("sge.window.getHeight", sge.window.getHeight() == 500)
end

local unit = {}

function unit.test()
    print("unit testing..")
    test_window()
end

return unit