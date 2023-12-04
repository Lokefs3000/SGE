local targetTest = 0

local test1_renderCount = 2500
local test1_performance = {}
local test1_avgperf = {}
local test1_timeLast = 0
local test1_frames = 0
local test1_duration = 0

function sge_load()
    targetTest = 1
end

function sge_exit()
    
end

local function avgPerf()
    local avg = 0

    for i = 1, #test1_avgperf do
        avg = avg + test1_avgperf[i]
    end
    avg = avg / #test1_avgperf

    test1_avgperf = {}
    table.insert(test1_performance, {count = test1_renderCount, framerate = avg, frames = test1_frames, time = test1_timeLast})
    test1_timeLast = 0
    test1_frames = 0
end

function sge_update()
    if targetTest == 0 then

        return
    end
    if targetTest == 1 then
        if test1_duration > 70 then
            avgPerf()
            print("finished")
            print("result:")
            targetTest = 0
            for i = 1, #test1_performance do
                print("     " .. i .. ":")
                print("          count: " .. test1_performance[i].count)
                print("          framerate: " .. test1_performance[i].framerate)
                print("          frames: " .. test1_performance[i].frames)
                print("          time: " .. test1_performance[i].time)
            end
            return
        end
        if test1_duration > 60 then
            if test1_renderCount ~= 100000 then
                print("increasing to 100000")
                avgPerf()
            end
            test1_renderCount = 100000
            return
        end
        if test1_duration > 50 then
            if test1_renderCount ~= 50000 then
                print("increasing to 50000")
                avgPerf()
            end
            test1_renderCount = 50000
            return
        end
        if test1_duration > 40 then
            if test1_renderCount ~= 30000 then
                print("increasing to 30000")
                avgPerf()
            end
            test1_renderCount = 30000
            return
        end
        if test1_duration > 30 then
            if test1_renderCount ~= 20000 then
                print("increasing to 20000")
                avgPerf()
            end
            test1_renderCount = 20000
            return
        end
        if test1_duration > 20 then
            if test1_renderCount ~= 10000 then
                print("increasing to 10000")
                avgPerf()
            end
            test1_renderCount = 10000
            return
        end
        if test1_duration > 10 then
            if test1_renderCount ~= 5000 then
                print("increasing to 5000")
                avgPerf()
            end
            test1_renderCount = 5000
            return
        end
    end
end

function sge_render()
    if targetTest == 0 then

        return
    end
    if targetTest == 1 then
        sge.renderer.clear(Vector4.new(0, 0, 0, 1))

        test1_duration = test1_duration + math.min(sge.time.delta(), 10)
        test1_timeLast = test1_timeLast + math.min(sge.time.delta(), 10)

        test1_frames = test1_frames + 1

        for i = 1, test1_renderCount do
            sge.renderer.drawRect(Vector4.new(math.random(0.00, 1.00), math.random(0.00, 1.00), math.random(0.00, 1.00), math.random(0.00, 1.00)), math.random(-540, 540), math.random(-360, 360), 100, 100);
        end

        sge.renderer.draw()

        table.insert(test1_avgperf, 1 / sge.time.delta())

        --sge.debug.text(tostring(1 / sge.time.delta()), 0xffffff, 0, 0)
        --sge.debug.text(tostring(test1_duration), 0xffffff, 0, 24)
        return
    end
end