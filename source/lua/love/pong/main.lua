-- PONG game implementation in `lua` language using `love2d` framework
APP_IS_DEBUG = true

APP_QUIT_READY = false
APP_WINDOW_SCALE = 1

function app_window_rect( mult )

    local function update_window_rect( mult )
        local scalar = 40
        local dim_x = 16
        local dim_y = 9
        mult = math.max( mult, 0.1 )
        APP_WINDOW_SCALE = mult
        APP_WINDOW_W = scalar * dim_x * mult
        APP_WINDOW_H = scalar * dim_y * mult
        love.window.setMode( APP_WINDOW_W, APP_WINDOW_H )
        app_handle_resize( APP_WINDOW_W, APP_WINDOW_H )
    end

    if mult ~= nil then update_window_rect( mult ) end

    return APP_WINDOW_W, APP_WINDOW_H
end

function APP_ASSERT( success, message )
    if APP_IS_DEBUG and not success then error( message or "APP_ASSERT failed", 2 ) end
    return success
end

function APP_DEBUG( message )
    if APP_IS_DEBUG then print( message ) end
end

function app_quit( )
    love.event.quit( )
end

function app_handle_resize( w, h )
    APP_BALL_SIZE = 8 * APP_WINDOW_SCALE
end

function hex_color( r, g, b )
    APP_ASSERT( 0x00 <= r and r <= 0xFF, "r is out of range" )
    APP_ASSERT( 0x00 <= g and g <= 0xFF, "g is out of range" )
    APP_ASSERT( 0x00 <= b and b <= 0xFF, "b is out of range" )

    return r / 0xFF, g / 0xFF, b / 0xFF
end

function love.load( )
    w, h = app_window_rect( 2 )
    app_handle_resize( w, h )
    APP_ASSERT( APP_WINDOW_W ~= nil and APP_WINDOW_H ~= nil, "either window width or height not set" )
end

function love.update( dt )
    w, h = app_window_rect( )
end

function love.resize( w, h )
    app_hangle_resize( w, h )
end

function love.draw( )
    love.graphics.setBackgroundColor( hex_color( 0x24, 0x32, 0x28 ))
    w, h = app_window_rect( )
    love.graphics.rectangle( "fill", w / 2 - APP_BALL_SIZE / 2,  h / 2 - APP_BALL_SIZE / 2, APP_BALL_SIZE, APP_BALL_SIZE )
end

function love.keypressed( key )
    APP_DEBUG( string.format( "keypressed key: %s", key))

    if key == "escape" then app_quit( ) end
    if key == "kp+" then app_window_rect( APP_WINDOW_SCALE + 0.1 ) end
    if key == "kp-" then app_window_rect( APP_WINDOW_SCALE - 0.1 ) end
end

function love.joystickhat( joy, hat, dir )
    APP_DEBUG( string.format("joystick hat #%d shifted to direction: %s", hat, dir))

    if dir == 'd' then APP_QUIT_READY = true
    else APP_QUIT_READY = false end
end

function love.joystickpressed( joystick, button )
    APP_DEBUG( string.format("joystick fired raw button number: %d", button))

    if button == 10 and APP_QUIT_READY then app_quit( ) end
end
