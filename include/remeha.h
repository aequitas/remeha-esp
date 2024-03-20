#include <stdint.h>

// https://gcc.gnu.org/onlinedocs/gcc/Structure-Layout-Pragmas.html
#pragma pack(1)

typedef struct {
  int16_t   aanvoer_temperature_c;
  int16_t   retour_temperature_c;
  int16_t   zonneboiler_temperature_c;
  int16_t   buiten_temperature_c;
  int16_t   boiler_temperature_c;
  int16_t   not_in_use1;
  int16_t   automaat_temperature_c;
  int16_t   ruimte_temperature_c;
  int16_t   cv_setpoint_c;
  int16_t   ww_setpoint_c;
  int16_t   ruimte_setpoint_c;
  uint16_t   ventilator_setpoint_rpm;
  uint16_t   ventilator_touren_rpm;
  // 26 33

  uint8_t    ionizatie_current_a;
  int16_t   intern_setpoint_c;
  // 3 36

  uint8_t    vermogen_beschikbaar_percent;
  uint8_t    pomp_percent;
  int8_t    not_in_use2;
  uint8_t    vermogen_gewenst_percent;
  uint8_t    vermogen_geleverd_percent;
  int8_t    not_in_use3;
  int8_t    not_in_use4;
  // 7 43

  // unsigned
  uint8_t   demand_source_bits;
  uint8_t   input_bits;
  uint8_t   valve_bits;
  uint8_t   pump_bits;
  // 4 47

  int8_t    state;
  int8_t    lockout;
  int8_t    blocking;
  int8_t    substate;
  int8_t    not_in_use6;
  int8_t    not_in_use7;
  int8_t    not_in_use8;
  int8_t    not_in_use9;
  int8_t    not_in_use10;
  uint8_t    druk_bar;
  int8_t    hru_bits;
  // 11 58

  int16_t   control_temp;
  int16_t   ww_flowrate;
  // 4 62

  int8_t    not_in_use11;
  int8_t    solar_temp_c;
  int8_t    not_in_use17;
  int8_t    not_in_use18;
  int8_t    not_in_use19;
  int8_t    not_in_use20;
  int8_t    not_in_use21;
  int8_t    not_in_use22;
  int8_t    not_in_use23;
} remehaStatus ;

typedef struct remehaPayload {
  int8_t header[6];
  remehaStatus status;
} remehaPayload;

typedef struct remehaPacket{
  uint8_t start_transmission;
  // remehaPayload payload;
  remehaPayload payload;

  uint16_t   crc;
  uint8_t    end_of_transmission;
} remehaPacket;

typedef struct {
  int8_t    start_transmission;
  int16_t   x2;
  int8_t    x3;
  int8_t    x4;
  int16_t   x5;
  // 7

  int16_t   aanvoer_temperature_c;
  int16_t   retour_temperature_c;
  int16_t   zonneboiler_temperature_c;
  int16_t   buiten_temperature_c;
  int16_t   boiler_temperature_c;
  int16_t   not_in_use1;
  int16_t   automaat_temperature_c;
  int16_t   ruimte_temperature_c;
  int16_t   cv_setpoint_c;
  int16_t   ww_setpoint_c;
  int16_t   ruimte_setpoint_c;
  int16_t   ventilator_setpoint_rpm;
  int16_t   ventilator_touren_rpm;
  // 26 33

  int8_t    ionizatie_current_a;
  int16_t   intern_setpoint_c;
  // 3 36

  int8_t    vermogen_beschikbaar_percent;
  int8_t    pomp_percent;
  int8_t    not_in_use2;
  int8_t    vermogen_gewenst_percent;
  int8_t    vermogen_geleverd_percent;
  int8_t    not_in_use3;
  int8_t    not_in_use4;
  // 7 43

  // unsigned
  uint8_t   demand_source_bits;
  uint8_t   input_bits;
  uint8_t   valve_bits;
  uint8_t   pump_bits;
  // 4 47

  int8_t    state;
  int8_t    lockout;
  int8_t    blocking;
  int8_t    substate;
  int8_t    not_in_use6;
  int8_t    not_in_use7;
  int8_t    not_in_use8;
  int8_t    not_in_use9;
  int8_t    not_in_use10;
  int8_t    druk_bar;
  int8_t    hru_bits;
  // 11 58

  int16_t   control_temp;
  int16_t   ww_flowrate;
  // 4 62

  int8_t    not_in_use11;
  int8_t    solar_temp_c;
  int8_t    not_in_use17;
  int8_t    not_in_use18;
  int8_t    not_in_use19;
  int8_t    not_in_use20;
  int8_t    not_in_use21;
  int8_t    not_in_use22;
  int8_t    not_in_use23;
  // 9 71

  int16_t   crc;
  int8_t    end_of_transmission;
  // 3 74

  // = 74
} remehaData;

const char* remeha_state[] = {
  "rust",
  "ketel_start",
  "brander_start",
  "brander_cv",
  "brander_sww",
  "brander_stop",
  "ketel_stop",
  "-",
  "regelstop",
  "blokkering",

  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "deairation",
};

const char* remeha_substate[64] = {
  "rust",
  "antipendel",
  "driewegklep_aansturen",
  "start_pomp",
  "wacht_temp_branderstart",
  "-",
  "-",
  "-",
  "-",
  "-",

  "open_rookgasklep",
  "verhogen_toerental_vent",
  "-",
  "voorventileren",
  "wacht_release_signal",
  "brander_aan",
  "-",
  "voorontsteking",
  "hoofdontsteking",
  "vlamdetectie",

  "tussenventileren",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",

  "temperatuurregeling",
  "begrensde_temperatuurregeling",
  "vermogensregeling",
  "stijgingsbeveiliging_terugmoduleren",
  "stijgingsbeveiliging_laaglast",
  "stijgingsbeveiliging_blokkering",
  "opmoduleren_vlambeveiliging",
  "temperatuur_stabilisatie",
  "koude_start",
  "-",

  "brander_uit",
  "naventileren",
  "sluit_rookgasklep",
  "recirculatie_beveiliging",
  "stop_ventilator",
  "-",
  "-",
  "-",
  "-",
  "-",

  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",

  "pomp_nadraaien",
  "pomp_uit",
  "driewegklep_aansturen",
  "start_antipendel",
};

const char* remeha_blocking[26] = {
  "parameterfout",
  "max_aanvoertemp_overschreden",
  "max_stijging_aanvoertemp_overschreden",
  "-",
  "-",
  "-",
  "-",
  "max_delta_t_overschreden",
  "geen_vrijgavesignaal",
  "fase_nul_netspanning_omgewisseld",

  "blokkerende_ingang_actief",
  "blokkerende_ingang_vorstbeveiliging_actief",
  "-",
  "communicatiefout_scu",
  "waterdruk_te_laag",
  "gasdruk_te_laag",
  "configuratiefout_su_niet_herkend",
  "configuratiefout_default_parametertabel_niet_op_orde",
  "configuratiefout_psu_niet_herkend",
  "configuratiefout_df_du_onbekend",

  "configuratieprocedure_actief",
  "communicatiefout_su",
  "vlamwegval_tijdens_bedrijf",
  "-",
  "-",
  "interne_fout_su",
};

const char* remeha_storingen[41] = {
  "psu_niet_gevonden",
  "veiligheidsparameters_niet_in_orde",
  "aanvoer_temp_sensor_kortgesloten",
  "aanvoer_temp_sensor_open",
  "temperatuur_warmtewisselaar_te_laag"
  "temperatuur_warmtewisselaar_te_hoog"
  "retour_temp_sensor_kortgesloten",
  "retour_temp_sensor_open",
  "retour_temp_to_laag",
  "retour_temp_to_hoog",

  "te_veel_verschil_aanvoer_retour",
  "te_veel_verschil_aanvoer_retour",
  "temp_warmtewisselaar_boven_normaal_bereik",
  "-",
  "5_mislukte_branderstarts",
  "-",
  "vals_vlamsignaal",
  "gasklep_storing_su",
  "-",
  "-",

  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",
  "-",

  "-",
  "-",
  "-",
  "-",
  "ventilator_storing",
  "aanvoer_retour_verwisseld",
  "5_keer_vlamverlies",
  "communicatiefout_su",
  "communicatiefout_scu",
  "blokkerende_ingang_vergrendelende_mode",

  "hru_wtw_unit_testfout",
};

const char* remeha_bits_demand_source[8] = {
  "modulating_controller_connected",
  "heat_demand_modulating_controller",
  "heat_demand_on_off_controller",
  "heat_demand_frost_protection",
  "dhw_eco",
  "dhw_blocking",
  "heat_demand_anti_legionella",
  "heat_demand_dhw",
};

const char* remeha_bits_input[8] = {
  "input_shutdown",
  "input_release",
  "ionisation",
  "flow_switch_detecting_dhw",
  "",
  "min_gas_pressure",
  "ch_enable",
  "dhw_enable",
};

const char* remeha_bits_valve[8] = {
  "gas_valve",
  "",
  "ignition",
  "three_way_valve_position",
  "ext_three_way_valve",
  "",
  "ext_gas_valve",
  "",
};

// Gas Valve[0]: 1:Closed
// Ignition[2]: 0:Off
// 3-Way Valve[3]: 1:DHW
// External 3-Way Valve[4]: 0:Open
// External Gas Valve[6]: 0:Open

const char* remeha_bits_pump[8] = {
  "pump",
  "boiler_pump",
  "ext_ch_pump",
  "",
  "status_report",
  "",
  "",
  "openterm_smartpower",
};

// Pump[0]: 0:Off
// Calorifier Pump[1]: 0:Open
// External CH Pump[2]: 0:Off
// Status Report[4]: 1:Closed
// Opentherm Smart Power[7]: 0:Off