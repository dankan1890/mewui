-- license:BSD-3-Clause
-- copyright-holders:Carl
--
-- json cheat file format
-- [{
--   "desc": "text",
--   "parameter": {
--     "min": "minval(0)",
--     "max": "maxval(numitems)",
--     "step": "stepval(1)",
--     "item" [{
--       "value": "itemval(index*stepval+minval)",
--       "text": "text"
--     },
--     ... ]
--   },
--   "space": {
--     "varname": {
--       "tag": "tag",
--       "type": "program|data|io"
--     },
--     ...
--   },
--   "screen": {
--     "varname": "tag",
--     ...
--   },
--   "region": {
--     "varname": "tag",
--     ...
--   },
--   "ram": {
--     "varname": "tag",
--     ...
--   },
--   "script": {
--     "on|off|run|change": "script",
--      ...
--   },
--   "comment": "text"
-- },
-- ... ]
--
-- Scripts are lua scripts with a limited api. Most library functions are unavailable.
-- Like the XML cheats, param is the current parameter value and variables are shared between scripts within a cheat
-- Differences from XML cheats:
-- - actions are only one line which include the entire script
-- - "condexpr" is replaced with lua control statements (if-then-else-end)
-- - variables are only limited by the limits of the lua interperter, you can have strings and tables
-- - the address spaces in the "space" blocks are accessible to the script if included,
--      same with regions (the "m" space in debug expr)
-- - frame is replaced by screen:frame_number() so if you use frame a screen needs to be in the device section
-- - output is a function and argindex isn't supported, output args need to be explicit and a screen device
--      must be provided

local exports = {}
exports.name = "cheat"
exports.version = "0.0.1"
exports.description = "Cheat plugin"
exports.license = "The BSD 3-Clause License"
exports.author = { name = "Carl" }

local cheat = exports

function cheat.startplugin()
	local cheats = {}
	local output = {}
	local line = 0
	local start_time = 0
	local stop = true

	local function load_cheats()
		local filename = emu.romname()
		local newcheats = {}
		local file = emu.file(manager:machine():options().entries.cheatpath:value():gsub("([^;]+)", "%1;%1/cheat") , 1)
		if emu.softname() ~= "" then
			for name, image in pairs(manager:machine().images) do
				if image:exists() and image:software_list_name() ~= "" then
					filename = image:software_list_name() .. "/" .. emu.softname()
				end
			end
		end
		local function add(addcheats)
			if not next(newcheats) then
				newcheats = addcheats
			else
				for num, cheat in pairs(addcheats) do
					newcheats[#newcheats + 1] = cheat
				end
			end
		end
		local json = require("json")
		local ret = file:open(filename .. ".json")
		while not ret do
			add(json.parse(file:read(file:size())))
			ret = file:open_next()
		end
		local xml = require("cheat/xml_conv")
		ret = file:open(filename .. ".xml")
		while not ret do
			add(xml.conv_cheat(file:read(file:size())))
			ret = file:open_next()
		end
		local simp = require("cheat/simple_conv")
		ret = file:open("cheat.simple")
		while not ret do
			add(simp.conv_cheat(filename, file:read(file:size())))
			ret = file:open_next()
		end
		return newcheats
	end

	local function draw_text(screen, x, y, color, form, ...)
		local str = form:format(...)
		if y == "auto" then
			y = line
			line = line + 1
		end
		if not screen then
			emu.print_verbose("draw_text: invalid screen")
			return
		end
		if type(x) == "string" then
			y = y * mame_manager:ui():get_line_height()
		end
		output[#output + 1] = { type = "text", scr = screen, x = x, y = y, str = str, color = color }
	end

	local function draw_line(screen, x1, y1, x2, y2, color)
		if not screen then
			emu.print_verbose("draw_line: invalid screen")
			return
		end
		output[#output + 1] = { type = "line", scr = screen, x1 = x1, x2 = x2, y1 = y1, y2 = y2, color = color }
	end

	local function draw_box(screen, x1, y1, x2, y2, bgcolor, linecolor)
		if not screen then
			emu.print_verbose("draw_box: invalid screen")
			return
		end
		output[#output + 1] = { type = "box", scr = screen, x1 = x1, x2 = x2, y1 = y1, y2 = y2, bgcolor = bgcolor, linecolor = linecolor }
	end

	local function tobcd(val)
		local result = 0
		local shift = 0
		while val ~= 0 do
			result = result + ((val % 10) << shift)
			val = val / 10
			shift = shift + 4
		end
		return result
	end

	local function frombcd(val)
		local result = 0
		local mul = 1
		while val ~= 0 do
			result = result + ((val % 16) * mul)
			val = val >> 4
			mul = mul * 10
		end
		return result
	end

	local function time()
		return emu.time() - start_time
	end


	local function parse_cheat(cheat)
		cheat.cheat_env = { draw_text = draw_text,
					draw_line = draw_line,
					draw_box = draw_box,
					tobcd = tobcd,
					frombcd = frombcd,
					pairs = pairs,
					ipairs = ipairs,
					outputs = manager:machine():outputs(),
					time = time,
						table =
					{ insert = table.insert,
						  remove = table.remove } }
		cheat.enabled = false
		-- verify scripts are valid first
		if not cheat.script then
			return
		end
		for name, script in pairs(cheat.script) do
			script, err = load(script, cheat.desc .. name, "t", cheat.cheat_env)
			if not script then
				emu.print_verbose("error loading cheat script: " .. cheat.desc .. " " .. err)
				cheat.desc = cheat.desc .. " error"
				cheat.script = nil
				return
			end
			cheat.script[name] = script
		end
		-- initialize temp[0-9] for backward compatbility reasons
		for i = 0, 9 do
			cheat.cheat_env["temp" .. i] = 0
		end
		if cheat.space then
			for name, space in pairs(cheat.space) do
				local cpu, mem
				cpu = manager:machine().devices[space.tag]
				if not cpu then
					emu.print_verbose("error loading cheat script: " .. cheat.desc .. " missing device " .. space.tag)
					cheat.desc = cheat.desc .. " error"
					cheat.script = nil
					return
				end
				if space.type then
					mem = cpu.spaces[space.type]
				else
					space.type = "program"
					mem = cpu.spaces["program"]
				end
				if not mem then
					emu.print_verbose("error loading cheat script: " .. cheat.desc .. " missing space " .. space.type)
					cheat.desc = cheat.desc .. " error"
					cheat.script = nil
					return
				end
				cheat.cheat_env[name] = mem
			end
		end
		if cheat.screen then
			for name, screen in pairs(cheat.screen) do
				local scr = manager:machine().screens[screen]
				if not scr then
					local tag
					tag, scr = next(manager:machine().screens) -- get any screen
				end
				cheat.cheat_env[name] = scr
			end
		end
		if cheat.region then
			for name, region in pairs(cheat.region) do
				local mem = manager:machine():memory().regions[region]
				if not mem then
					emu.print_verbose("error loading cheat script: " .. cheat.desc .. " missing region " .. region)
					cheat.desc = cheat.desc .. " error"
					cheat.script = nil
					return
				end
				cheat.cheat_env[name] = mem
			end
		end
		if cheat.ram then
			for name, tag in pairs(cheat.ram) do
				local ram = manager:machine().devices[tag]
				if not ram then
					emu.print_verbose("error loading cheat script: " .. cheat.desc .. " missing ram device " .. ram)
					cheat.desc = cheat.desc .. " error"
					cheat.script = nil
					return
				end
				cheat.cheat_env[name] = emu.item(ram.items["0/m_pointer"])
			end
		end
		local param = cheat.parameter
		if not param then
			return
		end
		param.min = tonumber(param.min) or 0
		param.max = tonumber(param.max) or #param.item
		param.step = tonumber(param.step) or 1
		if param.item then
			for count, item in pairs(param.item) do
				if not item.value then
					item.value = (count * param.step) + param.min
				else
					item.value = tonumber(item.value)
				end
			end
			param.last = #param.item
		else
			param.last = ((param.max - param.min) / param.step) + 1
		end
		param.index = 0
		param.value = param.min
		cheat.cheat_env.param = param.min
	end

	local function menu_populate()
		local menu = {}
		for num, cheat in ipairs(cheats) do
			menu[num] = {}
			menu[num][1] = cheat.desc
			if not cheat.parameter then
				if not cheat.script then
					if cheat.desc == "" then
						menu[num][1] = "---"
					end
					menu[num][2] = ""
					menu[num][3] = "off"
				elseif not cheat.script.run and not cheat.script.off then
					menu[num][2] = "Set"
					menu[num][3] = 0
				else
					if cheat.enabled then
						menu[num][2] = "On"
						menu[num][3] = "l"
					else
						menu[num][2] = "Off"
						menu[num][3] = "r"
					end
				end
			else
				if cheat.parameter.index == 0 then
					if not cheat.script.run and not cheat.script.off then
						menu[num][2] = "Set"
					else
						menu[num][2] = "Off"
					end
					menu[num][3] = "r"
				else
					if cheat.parameter.item then
						menu[num][2] = cheat.parameter.item[cheat.parameter.index].text
					else
						menu[num][2] = cheat.parameter.value
					end
					menu[num][3] = "l"
					if cheat.parameter.index < cheat.parameter.last then
						menu[num][3] = "lr"
					end
				end
			end
		end
		menu[#menu + 1] = {"---", "", 0}
		menu[#menu + 1] = {"Reset All", "", 0}
		menu[#menu + 1] = {"Reload All", "", 0}
		return menu
	end

	local function menu_callback(index, event)
		manager:machine():popmessage()
		if index > #cheats and event == "select" then
			index = index - #cheats
			if index == 2 then
				for num, cheat in pairs(cheats) do
					if cheat.enabled and cheat.script.off then
						cheat.script.off()
					end
					cheat.enabled = false
					if cheat.parameter then
						cheat.parameter.value = cheat.parameter.min
						cheat.parameter.index = 0
					end
				end
			elseif index == 3 then
				for num, cheat in pairs(cheats) do
					if cheat.enabled and cheat.script.off then
						cheat.script.off()
					end
				end
				cheats = load_cheats()
				for num, cheat in pairs(cheats) do
					parse_cheat(cheat)
				end
			end
			return true
		end

		local function param_calc(param)
			if param.item then
				if not param.item[param.index] then -- uh oh
					param.index = 1
				end
				param.value = param.item[param.index].value
				return
			end
			param.value = param.min + (param.step * (param.index - 1))
			if param.value > param.max then
				param.value = param.max
			end
		end

		local cheat = cheats[index]
		if not cheat then
			return false
		end
		if event == "up" or event == "down" or event == "comment" then
			if cheat.comment then
				manager:machine():popmessage("Cheat Comment:\n" .. cheat.comment)
			end
		elseif event == "left" then
			if cheat.parameter then
				local param = cheat.parameter
				if param.index == 1 then
					param.index = 0
					cheat.enabled = false
					cheat.cheat_env.param = param.min
					if cheat.script.off then
						cheat.script.off()
					end
					return true
				elseif param.index == 0 then
					return false
				end
				param.index = param.index - 1
				param_calc(param)
				cheat.cheat_env.param = param.value
				if cheat.script.change and (cheat.script.run or cheat.script.off) then
					cheat.script.change()
				end
				return true
			else
				if not cheat.script.run and not cheat.script.off then
					return false
				end
				if cheat.enabled then
					cheat.enabled = false
					if cheat.script.off then
						cheat.script.off()
					end
					return true
				end
				return false
			end
		elseif event == "right" then
			if cheat.parameter then
				local param = cheat.parameter
				if param.index == 0 then
					cheat.enabled = true
					if cheat.script.on then
						cheat.script.on()
					end
				elseif param.index == param.last then
					return false
				end
				param.index = param.index + 1
				param_calc(param)
				cheat.cheat_env.param = param.value
				if cheat.script.change and (cheat.script.run or cheat.script.off) then
					cheat.script.change()
				end
				return true
			else
				if not cheat.script.run and not cheat.script.off then
					return false
				end
				if not cheat.enabled then
					cheat.enabled = true
					if cheat.script.on then
						cheat.script.on()
					end
					return true
				end
				return false
			end
		elseif event == "select" then
			if not cheat.script.run and not cheat.script.off then
				if cheat.parameter and cheat.script.change and cheat.parameter.index ~= 0 then
					param_calc(cheat.parameter)
					cheat.cheat_env.param = cheat.parameter.value
					cheat.script.change()
					local subtext
					if cheat.parameter.item then
						subtext = cheat.parameter.item[cheat.parameter.index]
					else
						subtext = cheat.parameter.value
					end
					manager:machine():popmessage("Activated: " .. cheat.desc .. " = " .. subtext)
				elseif not cheat.parameter and cheat.script.on then
					cheat.script.on()
					manager:machine():popmessage("Activated: " .. cheat.desc)
				end
			end
		end
		return false
	end

	emu.register_menu(function(index, event)
				return menu_callback(index, event)
			  end,
			  function()
				return menu_populate()
			  end, "Cheat")

	emu.register_start(function()
		if not stop then
			return
		end
		stop = false
		start_time = emu.time()
		cheats = load_cheats()
		local json = require("json")
		local file = io.open(manager:machine():options().entries.cheatpath:value():match("([^;]+)") .. "/output.json", "w")
		if file then
			file:write(json.stringify(cheats, {indent = true}))
			file:close()
		end
		for num, cheat in pairs(cheats) do
			parse_cheat(cheat)
		end
	end)

	emu.register_stop(function()
		stop = true
	end)

	emu.register_frame(function()
		if stop then
			return
		end
		for num, cheat in pairs(cheats) do
			if cheat.enabled and cheat.script.run then
				cheat.script.run()
			end
		end
	end)

	emu.register_frame_done(function()
		if stop then
			return
		end
		line = 0
		for num, draw in pairs(output) do
			if draw.type == "text" then
				if not draw.color then
					draw.scr:draw_text(draw.x, draw.y, draw.str)
				else
					draw.scr:draw_text(draw.x, draw.y, draw.str, draw.color)
				end
			elseif draw.type == "line" then
				draw.scr:draw_line(draw.x1, draw.y1, draw.x2, draw.y2, draw.color)
			elseif draw.type == "box" then
				draw.scr:draw_box(draw.x1, draw.y1, draw.x2, draw.y2, draw.bgcolor, draw.linecolor)
			end
		end
		output = {}
	end)

	local ce = {}

	-- interface to script cheat engine
	function ce.inject(newcheat)
		cheats[#cheats + 1] = newcheat
		parse_cheat(newcheat)
		manager:machine():popmessage(newcheat.desc .. " added")
	end

	function ce.get(index)
		return cheats[index]
	end

	function ce.list()
		local list = {}
		for num, cheat in pairs(cheats) do
			list[num] = cheat.desc
		end
		return list
	end

	_G.ce = ce

end

return exports
