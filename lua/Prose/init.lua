-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Parser = require((...) .. "/Parser")

local collectgarbage = collectgarbage
local floor = math.floor
local pairs = pairs
local renderqueue = rainbow.renderqueue
local setmetatable = setmetatable
local table = table

local ProseScene = {
    __gc = function(self)
        renderqueue:erase_if_tag_starts_with(self.uid)
    end,
    __index = nil
}
ProseScene.__index = ProseScene

local function base62encode(value)
    local alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    local hash = {}
    repeat
        local r = (value % 62) + 1
        value = floor(value / 62)
        table.insert(hash, 1, alphabet:sub(r, r))
    until value == 0
    return table.concat(hash)
end

local function clear_table(t)
    for k,_ in pairs(t or {}) do
        t[k] = nil
    end
end

local function from_table(t)
    if type(t) ~= "table" or t.version < Parser.version then
        return nil
    end
    collectgarbage("stop")
    local uid = base62encode(rainbow.random(1, 2147483647)) .. '_'
    local res = Parser.create_resources(t.resources)
    local objs = Parser.create_nodes(uid, res, t.nodes)
    collectgarbage("restart")
    return setmetatable({
        uid = uid,
        objects = objs,
        resources = res
    }, ProseScene)
end

return {from_table = from_table}
