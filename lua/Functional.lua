-- Functions from functional programming languages such as Haskell and Scheme.
--
-- References:
--   https://bitbucket.org/luafunctional/lua-functional
--   http://haskell.org/ghc/docs/6.12.2/html/libraries/base-4.2.0.1/Data-List.html
--   http://lua-users.org/wiki/FunctionalLibrary
--   http://pragprog.com/magazines/2013-05/a-functional-introduction-to-lua
--
-- Copyright (c) 2013 Tommy Nguyen
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

local pairs = pairs

-- Auxiliary function for sum.
local function _add(a, b)
	return a + b
end

-- Auxiliary function for quotient.
local function _div(a, b)
	return a / b
end

-- Auxiliary function for foldl.
local function _foldl(op, z, list, i, final)
	if i == final then
		return z
	else
		return _foldl(op, op(z, list[i]), list, i + 1, final)
	end
end

-- Auxiliary function for foldr.
local function _foldr(op, z, list, i)
	if i == 0 then
		return z
	else
		return _foldr(op, op(list[i], z), list, i - 1)
	end
end

-- Auxiliary function for product.
local function _mul(a, b)
	return a * b
end

-- Auxiliary function for difference.
local function _sub(a, b)
	return a - b
end

-- Returns f ∘ g, i.e. the composition of two functions so that the result of
-- function g becomes the input to function f.
-- Example: printf = compose(io.write, string.format)
local function compose(f, g)
	return function(...)
		return f(g(...))
	end
end

-- Returns a copy of the list containing only the elements that satisfy the
-- predicate.
local function filter(pred, list)
	local filtered = {}
	local j = 0
	for i = 1, #list do
		local value = list[i]
		if pred(value) then
			j = j + 1
			filtered[j] = value
		end
	end
	return filtered
end

-- Returns a copy of the hash table containing only the elements that satisfy
-- the predicate.
local function filterh(pred, table)
	local filtered = {}
	for k, v in pairs(table) do
		if pred(v) then
			filtered[k] = v
		end
	end
	return filtered
end

-- Reduces the list using the binary operator, from left to right, with z as
-- starting value.
local function foldl(op, z, list)
	return _foldl(op, z, list, 1, #list + 1)
end

-- A variant of foldl that uses the leftmost element as starting value.
local function foldl1(op, list)
	return _foldl(op, list[1], list, 2, #list + 1)
end

-- Reduces the list using the binary operator, from right to left, with z as
-- starting value.
local function foldr(op, z, list)
	return _foldr(op, z, list, #list)
end

-- A variant of foldr that uses the rightmost element as starting value.
local function foldr1(op, list)
	local i = #list
	return _foldr(op, list[i], list, i - 1)
end

-- Returns a copy of the list with the function f applied to each element.
local function map(f, list)
	local mapped = {}
	for i = 1, #list do
		mapped[i] = f(list[i])
	end
	return mapped
end

-- Returns a copy of the hash table with the function f applied to each element.
local function maph(f, table)
	local mapped = {}
	for k, v in pairs(table) do
		mapped[k] = f(k, v)
	end
	return mapped
end

-- Computes the difference of a finite list of numbers; the first value being
-- the minuend.
local function difference(list)
	return foldl1(_sub, list)
end

-- Computes the product of a finite list of numbers.
local function product(list)
	return foldr1(_mul, list)
end

-- Computes the quotient of a finite list of numbers; the first value being the
-- numerator.
local function quotient(list)
	return foldl1(_div, list)
end

-- Computes the sum of a finite list of numbers.
local function sum(list)
	return foldr1(_add, list)
end

return {
	compose = compose,
	difference = difference,
	filter = filter,
	filterh = filterh,
	foldl = foldl,
	foldl1 = foldl1,
	foldr = foldr,
	foldr1 = foldr1,
	map = map,
	maph = maph,
	product = product,
	quotient = quotient,
	sum = sum
}
