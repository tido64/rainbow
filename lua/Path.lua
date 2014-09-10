-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local module_path = (...):match("(.*[./\\])[^./\\]+") or ""
local M = require(module_path .. "Math")

local sqrt = math.sqrt

local function clamp01(v)
    return M.clamp(v, 0.0, 1.0)
end

local Path = {
    Catmull_Rom = require(module_path .. "Splines/Catmull-Rom"),
    length = nil,
    __index = nil,
    iterate = nil,
    reset = nil,
    solve = nil
}

function Path.length(p)
    return sqrt(p[1] * p[1] + p[2] * p[2])
end

Path.__index = setmetatable(Path, {
    __call = function(Path, points, spline)
        return setmetatable({
            progress = 0.0,
            points = points,
            spline = spline,
        }, Path)
    end
})

function Path:iterate(t)
    local ds = self.spline.derivative(self.points, clamp01(self.progress))
    self.progress = self.progress + t / Path.length(ds)
    return self:solve(self.progress)
end

function Path:reset()
    self.progress = 0.0
end

function Path:solve(t)
    return self.spline.solve(self.points, clamp01(t))
end

return Path
