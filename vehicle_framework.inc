#include <a_samp>

#include <YSI_Game\y_vehicledata>

#if defined UGMP_Enable
    #include <ugmp>
#endif

// -- 
// Script-Side
// --


#if !defined MAX_NUMBER_PLATE
    #define MAX_NUMBER_PLATE 32
#endif

static const 
    DIMENSION_INTERIOR = 0,
    DIMENSION_VIRTUAL_WORLD = 1;

static 
    Vehicle_Model[MAX_VEHICLES],
    Vehicle_NumberPlate[MAX_VEHICLES][MAX_NUMBER_PLATE],
    Vehicle_RespawnDelay[MAX_VEHICLES],
    Vehicle_DimensionInfo[MAX_VEHICLES][2],
    Vehicle_ColorInfo[MAX_VEHICLES][2],
    Vehicle_Handler[MAX_VEHICLES],
    
    Iterator: Vehicle_Iter<MAX_VEHICLES>,
    Float: Vehicle_Health[MAX_VEHICLES],
    Float: Vehicle_Position[MAX_VEHICLES][4];
    

// --
// Params
// --

static const
    PARAMS_ENGINE = 0,
    PARAMS_LIGHTS = 1,
    PARAMS_ALARM = 2, 
    PARAMS_DOORS = 3,
    PARAMS_BONNET = 4,
    PARAMS_BOOT = 5,
    PARAMS_OBJECTIVE = 6;

static const
    WINDOWS_DRIVER = 0,
    WINDOWS_PASSENGER = 1,
    WINDOWS_REAR_LEFT = 2,
    WINDOWS_REAR_RIGHT = 3;

static
    Vehicle_Params[MAX_VEHICLES][7],
    Vehicle_Windows[MAX_VEHICLES][4];

/**
  * <summary> Creates a vehicle.</summary>
  * <param name="modelid">vehicle model (400-611)</param>
  * <param name="Float: x">x vehicle rotation</param>
  * <param name="Float: y">y vehicle rotation</param>
  * <param name="Float: z">z vehicle rotation</param>
  * <param name="Float: rotation">vehicle angle</param>
  * <param name="respawn_delay">The time(ms) when the vehicle will be respawned again.</param>
  * <param name="interior">interior id, if you want to create a vehicle inside the building</param>
*/
stock Vehicle_Create(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world) {
    new idx = Iter_Alloc(Vehicle_Iter);
    
    Vehicle_Model[idx] = modelid;
    Vehicle_Position[idx][0] = x;
    Vehicle_Position[idx][1] = y;
    Vehicle_Position[idx][2] = z;
    Vehicle_Position[idx][3] = rotation;
    Vehicle_ColorInfo[idx][0] = color1;
    Vehicle_ColorInfo[idx][1] = color2;
    Vehicle_RespawnDelay[idx] = respawn_delay;
    Vehicle_DimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_DimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_Handler[idx] = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay);

    SetVehicleVirtualWorld(Vehicle_Handler[idx], virtual_world);
    LinkVehicleToInterior(Vehicle_Handler[idx], interior);
}

/**
 * <summary> Creates a vehicle with additional params.</summary>
 * <param name="modelid">vehicle model (400-611)</param>
 * <param name="Float: x">x vehicle rotation</param>
 * <param name="Float: y">y vehicle rotation</param>
 * <param name="Float: z">z vehicle rotation</param>
 * <param name="Float: rotation">vehicle angle</param>
 * <param name="respawn_delay">The time(ms) when the vehicle will be respawned again.</param>
 * <param name="interior">interior id, if you want to create a vehicle inside the building</param>
 * <param name="Float: health">vehicle health (damage)</param>
 * <param name="plate">the text that will appear on the vehicle plates</param>
*/
stock Vehicle_CreateEx(modelid, Float: x, Float: y, Float: z, Float: rotation, color1, color2, respawn_delay, interior, virtual_world, Float: health, const plate[MAX_NUMBER_PLATE]) {
    new idx = Iter_Alloc(Vehicle_Iter);
    
    Vehicle_Model[idx] = modelid;
    Vehicle_Position[idx][0] = x;
    Vehicle_Position[idx][1] = y;
    Vehicle_Position[idx][2] = z;
    Vehicle_Position[idx][3] = rotation;
    Vehicle_ColorInfo[idx][0] = color1;
    Vehicle_ColorInfo[idx][1] = color2;
    Vehicle_RespawnDelay[idx] = respawn_delay;
    Vehicle_DimensionInfo[idx][DIMENSION_INTERIOR] = interior;
    Vehicle_DimensionInfo[idx][DIMENSION_VIRTUAL_WORLD] = virtual_world;
    Vehicle_Health[idx] = health;
    
    strcopy(Vehicle_NumberPlate[idx], plate);
    Vehicle_Handler[idx] = CreateVehicle(modelid, x, y, z, rotation, color1, color2, respawn_delay);
    
    SetVehicleHealth(Vehicle_Handler[idx], health);
    SetVehicleNumberPlate(Vehicle_Handler[idx], plate);
    SetVehicleVirtualWorld(Vehicle_Handler[idx], virtual_world);
    LinkVehicleToInterior(Vehicle_Handler[idx], interior);
}

// -- 
// Engine 
// --

enum e_ENGINE_STATES {
    E_ENGINE_STATE_OFF = 0,
    E_ENGINE_STATE_ON
};

/**
 * <summary> Gets the engine state.</summary>
 * <param name="vehicleid"> Vehicle from which we get engine state.</param>
 * <returns> Engine state. </returns>
 */

stock e_ENGINE_STATES: Vehicle_GetEngineState(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);

    return e_ENGINE_STATES: Vehicle_Params[vehicleid][PARAMS_ENGINE];
}

/**
 * <summary> Sets the engine state.</summary>
 * <param name="vehicleid"> Vehicle which we set the engine state.</param>
 * <param name="engine_state"> Engine state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetEngineState(vehicleid, e_ENGINE_STATES: engine_state) {

    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_ENGINE] = _:engine_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

// --
// Lights
// --

enum e_LIGHT_STATES {
    E_LIGHT_ON = 0,
    E_LIGHT_OFF
};

enum e_LIGHT_RUN_STATE {
    E_LIGHTS_OFF,
    E_LIGHTS_ON    
};

// internal

static stock encode_lights(front_left_light, front_right_light, back_lights) {
    return front_left_light | (front_right_light << 2) | (back_lights << 6);
}

static stock decode_lights(lights, &front_left_light, &front_right_light, &back_lights) {
    front_left_light = lights & 1;
    front_right_light = lights >> 2 & 1;
    back_lights = lights >> 6 & 1;
}

//

/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights state.</param>
 * <param name="left_lights"> Left Ligth state to set.</param>
 * <param name="right_lights"> Right Ligth state to set.</param>
 * <param name="back_lights"> Back Ligth state to set.</param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/


stock Vehicle_SetLightsState(vehicleid, e_LIGHT_STATES: left_lights, e_LIGHT_STATES: right_lights, e_LIGHT_STATES: back_lights) {
    new 
        lights, 
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    new const ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, encode_lights(_:left_lights, _:right_lights, _:back_lights), unnecessary_params);
    
    return ret;
}


/**
 * <summary> Gets the lights state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights state.</param>
 * <param name="&left_lights"> Left Ligth state to get.</param>
 * <param name="&right_lights"> Right Ligth state to get.</param>
 * <param name="&back_lights"> Back Ligth state to get.</param>
*/

stock Vehicle_GetLightsState(vehicleid, &e_LIGHT_STATES: left_lights, &e_LIGHT_STATES: right_lights, &e_LIGHT_STATES: back_lights) {
    new 
        lights, 
        e_LIGHT_STATES: lights_status[3],
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, lights, unnecessary_params);
    decode_lights(lights, lights_status[0], lights_status[1], lights_status[2]);
    
    left_lights = lights_status[0];
    right_lights = lights_status[1];
    back_lights = lights_status[2];

}


/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle which we set the lights running state.</param>
 * <param name="light_state"> Lights state to set.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetLightsRunState(vehicleid, e_LIGHT_RUN_STATE: light_state) {
        
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_LIGHTS] = _:light_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return ret;
}

/**
 * <summary> Sets the lights state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the lights running state.</param>
 * <returns> Lights' running state. </returns>
*/

stock e_LIGHT_RUN_STATE: Vehicle_GetLightsRunState(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return e_LIGHT_RUN_STATE: Vehicle_Params[vehicleid][PARAMS_LIGHTS];
}


// --
// Alarms
// --
enum e_ALARMS_STATE {
    E_ALARMS_OFF = 0,
    E_ALARMS_ON
};

/**
 * <summary> Sets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle which we set the alarms state.</param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetAlarms(vehicleid, e_ALARMS_STATE: alarm_state) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_ALARM] = _:alarm_state;
   
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return ret;
}

/**
 * <summary> Gets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the alarms state.</param>
 * <returns> Alarms' state. </returns>
*/

stock e_ALARMS_STATE: Vehicle_GetAlarms(vehicleid) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return e_ALARMS_STATE: Vehicle_Params[vehicleid][PARAMS_ALARM];
}

// ---
// Doors
// ---

enum e_DOOR_LOCK_STATES {
    E_DOOR_STATE_OFF = 0,
    E_DOOR_STATE_ON
};

enum e_DOOR_STATES {
    E_DOOR_STATE_NORMAL = 0,
    E_DOOR_STATE_OPEN = 1,
    E_DOOR_STATE_DAMAGED = 2,
    E_DOOR_STATE_OPEN_DAMAGED,
    E_DOOR_STATE_REMOVED
};

//--
static stock encode_doors(bonnet, boot, driver_door, passenger_door) {
    return bonnet | (boot << 8) | (driver_door << 16) | (passenger_door << 24);
}


static stock decode_doors(doors, &bonnet, &boot, &driver_door, &passenger_door) {
    bonnet = doors & 7;
    boot = doors >> 8 & 7;
    driver_door = doors >> 16 & 7;
    passenger_door = doors >> 24 & 7;
}
//

/**
 * <summary> Sets the doors state.</summary>
 * <param name="vehicleid"> Vehicle which we set the doors state.</param>
 * <param name="hood"> Hood state to set. </param>
 * <param name="trunk"> Trunk state to set. </param>
 * <param name="driver"> Driver-door state to set. </param>
 * <param name="passenger"> Passenger-door state to set. </param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/

stock Vehicle_SetDoorState(vehicleid, e_DOOR_STATES: hood, e_DOOR_STATES: trunk, e_DOOR_STATES: driver, e_DOOR_STATES: passenger) {
    new
        unnecessary_params[4],
        ret;

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params[0], encode_doors(hood, trunk, driver, passenger), unnecessary_params[2], unnecessary_params[3]);
    return ret;
}

/**
 * <summary> Gets the doors state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the doors state.</param>
 * <param name="&hood"> Hood state to get.</param>
 * <param name="&trunk"> Trunk state to get.</param>
 * <param name="&driver"> Driver-door state to get.</param>
 * <param name="&passenger"> Passenger-door state to get.</param>
*/

stock Vehicle_GetDoorState(vehicleid, &e_DOOR_STATES: hood, &e_DOOR_STATES: trunk, &e_DOOR_STATES: driver, &e_DOOR_STATES: passenger) {
    new 
        doors, 
        e_TIRE_STATUS: doors_status[4],
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, unnecessary_params, doors);
    decode_doors(doors, doors_status[0], doors_status[1], doors_status[2], doors_status[3]);
}

/**
 * <summary> Gets the doors' lock state.</summary>
 * <param name="vehicleid"> Vehicle from which we set the doors' lock state.</param>
 * <returns> Doors' lock state. </returns>
*/

stock e_DOOR_LOCK_STATES: Vehicle_GetDoorsLockState(vehicleid) {

    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    
    return e_DOOR_LOCK_STATES: Vehicle_Params[vehicleid][PARAMS_DOORS];
}

/**
 * <summary> Sets the alarms state.</summary>
 * <param name="vehicleid"> Vehicle which we set the doors' lock state.</param>
 * <param name="door_state"> Door's lock state to set. </param>
 * <returns> SetVehicleParamsEx values. </returns>
*/

stock Vehicle_SetDoorsLockState(vehicleid, e_DOOR_LOCK_STATES: door_state) {
    GetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    Vehicle_Params[vehicleid][PARAMS_DOORS] = _:door_state;
    new const ret = SetVehicleParamsEx(vehicleid, Vehicle_Params[vehicleid][PARAMS_ENGINE], Vehicle_Params[vehicleid][PARAMS_LIGHTS], Vehicle_Params[vehicleid][PARAMS_ALARM], Vehicle_Params[vehicleid][PARAMS_DOORS], Vehicle_Params[vehicleid][PARAMS_BONNET], Vehicle_Params[vehicleid][PARAMS_BOOT], Vehicle_Params[vehicleid][PARAMS_OBJECTIVE]);
    return ret;
}



// ---
// Tires
// ---

enum e_TIRE_STATUS {
    E_TIRE_INFLATED = 0,
    E_TIRE_POPPED
};

/**
 * <summary> Gets the tires state.</summary>
 * <param name="vehicleid"> Vehicle which we set the tires state.</param>
 * <param name="rear_right_tire"> Rear Right Tire state to set.</param>
 * <param name="front_right_tire"> Front Right Tire state to set.</param>
 * <param name="rear_left_tire"> Rear Left Tire state to set.</param>
 * <param name="front_left_tire"> Front Left Tire state to set.</param>
 * <returns> UpdateVehicleDamageStatus values. </returns>
*/

stock Vehicle_SetTireState(vehicleid, e_TIRE_STATUS: rear_right_tire, e_TIRE_STATUS: front_right_tire, e_TIRE_STATUS: rear_left_tire, e_TIRE_STATUS: front_left_tire) {    
    new
        unnecessary_params[4],
        ret;

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    ret = UpdateVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], encode_tires(_:rear_right_tire, _:front_right_tire, _:rear_left_tire, _:front_left_tire));

    return ret;
}

/**
 * <summary> Gets the tires state.</summary>
 * <param name="vehicleid"> Vehicle from which we get the tires state.</param>
 * <param name="&rear_right_tire"> Rear Right Tire state to get.</param>
 * <param name="&front_right_tire"> Front Right Tire state to get.</param>
 * <param name="&rear_left_tire"> Rear Left Tire state to get.</param>
 * <param name="&front_left_tire"> Front Left Tire state to get.</param>
*/

stock Vehicle_GetTireState(vehicleid, &e_TIRE_STATUS: rear_right_tire, &e_TIRE_STATUS: front_right_tire, &e_TIRE_STATUS: rear_left_tire, &e_TIRE_STATUS: front_left_tire) {    
    new 
        tires, 
        e_TIRE_STATUS: tires_status[4],
        unnecessary_params;

    GetVehicleDamageStatus(vehicleid, unnecessary_params, unnecessary_params, unnecessary_params, tires);
    decode_tires(tires, tires_status[0], tires_status[1], tires_status[2], tires_status[3]);
    
    rear_right_tire = tires_status[0];
    front_right_tire = tires_status[1];
    rear_left_tire = tires_status[2];
    front_left_tire = tires_status[3];

}

//--

static stock encode_tires(rear_right_tire, front_right_tire, rear_left_tire, front_left_tire)
{
    return rear_right_tire | (front_right_tire << 1) | (rear_left_tire << 2) | (front_left_tire << 3);
}

static stock decode_tires(tires, &rear_right_tire, &front_right_tire, &rear_left_tire, &front_left_tire)
{
    rear_right_tire = tires & 1;
    front_right_tire = tires >> 1 & 1;
    rear_left_tire = tires >> 2 & 1;
    front_left_tire = tires >> 3 & 1;
}

// -- 
// Plate
// --

/**
 * <summary> Gets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="plate"> Plate to get.</param>
 * <param name="len"> Size of the plate.</param>
*/

stock Vehicle_GetNumberPlate(vehicleid, plate[], len = sizeof(plate)) {
	format(plate, len, "%s", Vehicle_NumberPlate[vehicleid]);
}

/**
 * <summary> Sets the vehicle's number plate.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="plate"> Plate to set.</param>
*/

stock Vehicle_SetNumberPlate(vehicleid, const plate[]) {

	format(Vehicle_NumberPlate[vehicleid], MAX_NUMBER_PLATE, "%s", plate);
	new const ret = SetVehicleNumberPlate(vehicleid, Vehicle_NumberPlate[vehicleid]);
    return ret;
}

// --
// Windows 
// --

enum e_WINDOWS_STATES {
    E_WINDOW_NOT_SET = -1,
    E_WINDOW_OPENED = 0,
    E_WINDOW_CLOSED
};

/**
 * <summary> Sets the vehicle's windows states.</summary>
 * <param name="vehicleid"> Vehicle which we set the vehicle plate.</param>
 * <param name="driver"> Driver-window state to set.</param>
 * <param name="passenger"> Passenger-window state to set.</param>
 * <param name="backleft"> Backleft-window state to set.</param>
 * <param name="backright"> Backright-window state to gset.</param>
*/

stock Vehicle_SetWindows(vehicleid, e_WINDOWS_STATES: driver, e_WINDOWS_STATES: passenger, e_WINDOWS_STATES: backleft, e_WINDOWS_STATES: backright) {
    Vehicle_Windows[vehicleid][WINDOWS_DRIVER] = _:driver;
    Vehicle_Windows[vehicleid][WINDOWS_PASSENGER] = _:passenger;
    Vehicle_Windows[vehicleid][WINDOWS_REAR_LEFT] = _:backleft;
    Vehicle_Windows[vehicleid][WINDOWS_REAR_RIGHT] = _:backright;
    SetVehicleParamsCarWindows(vehicleid, Vehicle_Windows[vehicleid][WINDOWS_DRIVER], Vehicle_Windows[vehicleid][WINDOWS_PASSENGER], Vehicle_Windows[vehicleid][WINDOWS_REAR_LEFT], Vehicle_Windows[vehicleid][WINDOWS_REAR_RIGHT]);
}

/**
 * <summary> Gets the vehicle's windows states. </summary>
 * <param name="vehicleid"> Vehicle from which we get the vehicle plate.</param>
 * <param name="&driver"> Driver-window state to get.</param>
 * <param name="&passenger"> Passenger-window state to get.</param>
 * <param name="&backleft"> Backleft-window state to get.</param>
 * <param name="&backright"> Backright-window state to get.</param>
*/

stock Vehicle_GetWindows(vehicleid, &e_WINDOWS_STATES: driver, &e_WINDOWS_STATES: passenger, &e_WINDOWS_STATES: backleft, &e_WINDOWS_STATES: backright) {
    GetVehicleParamsCarWindows(vehicleid, Vehicle_Windows[vehicleid][WINDOWS_DRIVER], Vehicle_Windows[vehicleid][WINDOWS_PASSENGER], Vehicle_Windows[vehicleid][WINDOWS_REAR_LEFT], Vehicle_Windows[vehicleid][WINDOWS_REAR_RIGHT]);
    driver = e_WINDOWS_STATES: Vehicle_Windows[vehicleid][WINDOWS_DRIVER];
    passenger = e_WINDOWS_STATES: Vehicle_Windows[vehicleid][WINDOWS_PASSENGER];
    backleft = e_WINDOWS_STATES: Vehicle_Windows[vehicleid][WINDOWS_REAR_LEFT];
    backright = e_WINDOWS_STATES: Vehicle_Windows[vehicleid][WINDOWS_REAR_RIGHT];
}

// --
// Panels 
// --

/* Note: It seems that you can only read the value of the windshield. 
        Setting it does update the value on the server, 
        but it does not result 
        in any physical change on the vehicle.
*/

enum e_PANEL_STATES {
    E_PANEL_UNDAMAGED = 0,
    E_PANEL_CRUSHED = 1,
    E_PANEL_HANGING_LOOSE = 2,
    E_PANEL_REMOVED = 3
};

//--

static stock decode_panels(panels, &front_left_panel, &front_right_panel, &rear_left_panel, &rear_right_panel, &windshield, &front_bumper, &rear_bumper)
{
    front_left_panel = panels & 15;
    front_right_panel = panels >> 4 & 15;
    rear_left_panel = panels >> 8 & 15;
    rear_right_panel = panels >> 12 & 15;
    windshield = panels >> 16 & 15;
    front_bumper = panels >> 20 & 15;
    rear_bumper = panels >> 24 & 15;
}

static stock encode_panels(front_left_panel, front_right_panel, rear_left_panel, rear_right_panel, windshield, front_bumper, rear_bumper)
{
    return front_left_panel | (front_right_panel << 4) | (rear_left_panel << 8) | (rear_right_panel << 12) | (windshield << 16) | (front_bumper << 20) | (rear_bumper << 24);
}

//--

/**
 * <summary> Sets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting panel states to.</param>
 * <param name="front_left_panel"> Front left panel state to set.</param>
 * <param name="front_right_panel"> Front right panel state to set.</param>
 * <param name="back_left_panel"> Back left panel state to set.</param>
 * <param name="back_right_panel"> Back right state to set.</param>
*/

stock Vehicle_SetPanelStates(vehicleid, e_PANEL_STATES: front_left_panel, e_PANEL_STATES: front_right_panel, e_PANEL_STATES: back_left_panel, e_PANEL_STATES: back_right_panel) {
    new
        unnecessary_params[4],
        unnecessary_panels[7];

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    decode_panels(unnecessary_params[0], unnecessary_panels[0], unnecessary_panels[1], unnecessary_panels[2], unnecessary_panels[3], unnecessary_panels[4], unnecessary_panels[5], unnecessary_panels[6]);
    
    new ret = UpdateVehicleDamageStatus(vehicleid, encode_panels(_:front_left_panel, _:front_right_panel, _:back_left_panel, _:back_right_panel, unnecessary_panels[4], unnecessary_panels[5], unnecessary_panels[6]), unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    return ret;
}

/**
 * <summary> Gets the vehicle's panels' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are getting panel states from.</param>
 * <param name="&front_left_panel"> Front left panel state to get.</param>
 * <param name="&front_right_panel"> Front right panel state to get.</param>
 * <param name="&back_left_panel"> Back left panel state to get.</param>
 * <param name="&back_right_panel"> Back right state to get.</param>
*/

stock Vehicle_GetPanelStates(vehicleid, &e_PANEL_STATES: front_left_panel, &e_PANEL_STATES: front_right_panel, &e_PANEL_STATES: back_left_panel, &e_PANEL_STATES: back_right_panel) {
    new
        unnecessary_params[4],
        e_PANEL_STATES: panels_status[7];

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    decode_panels(unnecessary_params[0], panels_status[0], panels_status[1], panels_status[2], panels_status[3], panels_status[4], panels_status[5], panels_status[6]);

    front_left_panel = panels_status[0];
    front_right_panel = panels_status[1];
    back_left_panel = panels_status[2];
    back_right_panel = panels_status[3];

}

/**
 * <summary> Sets the vehicle's bumpers' states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting bumper states to.</param>
 * <param name="front_left_panel"> Front bumper state to set.</param>
 * <param name="back_left_panel"> Rear bumper state to set.</param>
*/

stock Vehicle_SetBumperStates(vehicleid, e_PANEL_STATES: front_bumper, e_PANEL_STATES: rear_bumper) {
    new
        unnecessary_params[4],
        unnecessary_panels[7];

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    decode_panels(unnecessary_params[0], unnecessary_panels[0], unnecessary_panels[1], unnecessary_panels[2], unnecessary_panels[3], unnecessary_panels[4], unnecessary_panels[5], unnecessary_panels[6]);
    
    new ret = UpdateVehicleDamageStatus(vehicleid, encode_panels(unnecessary_panels[0], unnecessary_panels[1], unnecessary_panels[2], unnecessary_panels[3], unnecessary_panels[4], _:front_bumper, _:rear_bumper), unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    return ret;
}

/**
 * <summary> Sets the vehicle's bumpers states. </summary>
 * <param name="vehicleid"> Id of vehicle we are setting bumper states from.</param>
 * <param name="&front_left_panel"> Front bumper state to get.</param>
 * <param name="&back_left_panel"> Rear bumper state to get.</param>
*/


stock Vehicle_GetBumperStates(vehicleid, &e_PANEL_STATES: front_bumper, &e_PANEL_STATES: rear_bumper) {
    new
        unnecessary_params[4],
        e_PANEL_STATES: panels_status[7];

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    decode_panels(unnecessary_params[0], panels_status[0], panels_status[1], panels_status[2], panels_status[3], panels_status[4], panels_status[5], panels_status[6]);

    front_bumper = panels_status[5];
    rear_bumper = panels_status[6];

}

/**
 * <summary> Gets the vehicle's windshield state. </summary>
 * <param name="vehicleid"> Id of vehicle we are getting winshield state from.</param>
 * <param name="&windshield"> Windshield state to get.</param>
*/

stock Vehicle_GetWindshieldState(vehicleid, &e_PANEL_STATES: windshield) {
    new
        unnecessary_params[4],
        e_PANEL_STATES: panels_status[7];

    GetVehicleDamageStatus(vehicleid, unnecessary_params[0], unnecessary_params[1], unnecessary_params[2], unnecessary_params[3]);
    decode_panels(unnecessary_params[0], panels_status[0], panels_status[1], panels_status[2], panels_status[3], panels_status[4], panels_status[5], panels_status[6]);

    windshield = panels_status[4];

}

// --