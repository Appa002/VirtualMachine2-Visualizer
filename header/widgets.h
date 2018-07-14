//
// Created by a_mod on 29.05.2018.
//

#ifndef VM_VISUALIZER_WIDGETS_H
#define VM_VISUALIZER_WIDGETS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <ncurses/curses.h>
#include <locale.h>
#include <values.h>
#include <errno.h>

#include "program.h"
#include "utils.h"
#include "io.h"
#include "helper.h"
#include "maths.h"

void write_keymap_line() {
    move(LINES - 1, 0);
    attrset(COLOR_PAIR(8));
    write_colored("q(quit)", 8);
    write_colored(" ", 0);
    write_colored("s(save)", 8);
    write_colored(" ", 0);
    write_colored("d(delete)", 8);
    write_colored(" ", 0);
    write_colored("a(add line)", 8);
    write_colored(" ", 0);
    write_colored("b(add line before)", 8);
    write_colored(" ", 0);
    write_colored("r(replace argument)", 8);
    write_colored(" ", 0);
    write_colored("up arrow(scroll up)", 8);
    write_colored(" ", 0);
    write_colored("down arrow(scroll down)", 8);
    write_colored(" ", 0);
    write_colored("left arrow(select left)", 8);
    write_colored(" ", 0);
    write_colored("right arrow(select right)", 8);
    attrset(COLOR_PAIR(0));
    move(0, 0);
    refresh();
}

void write_representation_line(uint32_t line_index, program_t* program, uint32_t element){
    line_t* line = program->lines[line_index];
    clear_line(LINES - 2);
    move(LINES - 2, 0);

    if(line_index >= program->line_amount)
        return;

    if(element - 2 > line->instruction_args_amount)
        element = 0;
    else
        element -= 2;

    if(line->instruction_args_amount == 0) {
        char instruction[256];
        sprintf(instruction, "Instruction: %s", line->instruction_name);
        write_colored(instruction, 9);
        write_colored(" ", 0);
        write_colored("No Args", 9);
        return;
    }

    char hex[256];
    char c[256];
    char udec[256];
    char sdec[256];
    char sign = (char) (line->instruction_args[element] >> 31 == 0 ? '+' : '-');
    char arg[256];
    char instruction[256];
    char asFloat[256];

    sprintf(hex, "Hex: %#010x", line->instruction_args[element]);
    sprintf(c, "Char: %c", line->instruction_args[element]);
    sprintf(udec, "Unsigned Dec: %u", line->instruction_args[element]);
    sprintf(sdec, "Signed Dec: %c%u", sign, line->instruction_args[element] & 0x7fff);
    sprintf(asFloat, "Float: %f", readIEEE754Float(line->instruction_args[element]));
    sprintf(arg, "Displaying for Arg: %u", element);
    sprintf(instruction, "Instruction: %s", line->instruction_name);

    write_colored(instruction, 9);
    write_colored(" ", 0);
    write_colored(arg, 9);
    write_colored(" ", 0);
    write_colored(hex, 9);
    write_colored(" ", 0);
    write_colored(c, 9);
    write_colored(" ", 0);
    write_colored(udec, 9);
    write_colored(" ", 0);
    write_colored(sdec, 9);
    write_colored(" ", 0);
    write_colored(asFloat, 9);

    attrset(COLOR_PAIR(0));
    refresh();
}

void write_line_at(uint32_t line_index, int y_pos, program_t *program) {
    if (line_index >= program->line_amount)
        return;

    move(y_pos, 0);

    char line_num_as_hex[32];
    sprintf(line_num_as_hex, "%#010x", program->lines[line_index]->line_address);

    write_colored(line_num_as_hex, 0);
    addstr(" ");
    write_colored(program->lines[line_index]->instruction_name, 0);
    addstr(" ");

    for (size_t j = 0; j < program->lines[line_index]->instruction_args_amount; j++) {
        char arg_as_hex[8 * 4];
        sprintf(arg_as_hex, "%#010x", program->lines[line_index]->instruction_args[j]);

        write_colored(arg_as_hex, 0);
        addstr(" ");
    }

    move(y_pos, 0);
}

void write_program(program_t *program, uint32_t start_line) {
    for (uint32_t i = start_line; i < program->line_amount && (i - start_line) < LINES - 2; i++) {
        write_line_at(i, (int)(i-start_line), program);
    }
    refresh();
}

bool save_scroll_to(uint32_t line_index, program_t *program) {
    if (line_index >= program->line_amount)
        return false;
    move(0, 0);
    clear();
    write_program(program, line_index);
    write_representation_line(line_index, program, 0);
    write_keymap_line();
    return true;
}

void move_cursor_up(uint32_t *line_index, program_t *program, int *cur_y_pos, const int *cur_x_pos) {
    if (*cur_y_pos - 1 < 0) {
        *cur_y_pos = 0;
        if (save_scroll_to(*line_index - 1, program))
            (*line_index)--;
    } else{
        move(*cur_y_pos - 1, *cur_x_pos);
        *cur_y_pos -= 1;
    }
}

void move_cursor_down(uint32_t *line_index, program_t *program, int* cur_y_pos, const int* cur_x_pos) {
    if (*cur_y_pos + 1 >= LINES - 2) {
        if (save_scroll_to(*line_index + 1, program))
            (*line_index)++;
        *cur_y_pos = LINES - 3;
        move(*cur_y_pos, *cur_x_pos);

    } else if(*cur_y_pos + 1){
        *cur_y_pos += 1;
        move(*cur_y_pos, *cur_x_pos);
    }
}

void mark_line_part(program_t *program, int y_pos, uint32_t line_index, int32_t *element) {
    write_line_at(line_index - 1, y_pos - 1, program);
    if(y_pos + 1 < LINES - 2)
        write_line_at(line_index + 1, y_pos + 1, program);

    if (*element < 0)
        *element = program->lines[line_index]->instruction_args_amount + 1;

    if (*element >= program->lines[line_index]->instruction_args_amount + 2)
        *element = 0;

    move(y_pos, 0);

    char line_num_as_hex[32];
    sprintf(line_num_as_hex, "%#010x", program->lines[line_index]->line_address);

    if_write_colored(*element == 0, line_num_as_hex, 6);
    addstr(" ");
    if_write_colored(*element == 1, program->lines[line_index]->instruction_name, 6);
    addstr(" ");

    for (size_t j = 0; j < program->lines[line_index]->instruction_args_amount; j++) {
        char arg_as_hex[8 * 4];
        sprintf(arg_as_hex, "%#010x", program->lines[line_index]->instruction_args[j]);

        if_write_colored(*element == 2 + j, arg_as_hex, 6);
        addstr(" ");
    }

    move(y_pos, 0);
}

void write_saved(){
    WINDOW* saved_state = dupwin(stdscr);

    write_center_box("Saved!", "Press <enter>", 5, 5, 1);

    get_key_press();
    overwrite(saved_state, stdscr);
}

void write_char_replace(uint32_t line_index, int element, program_t *program){
    WINDOW* saved_state = dupwin(stdscr);

    char* text = "Enter your char:";

    int largest_string_size = (int)strlen(text);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);
    write_center_box_line(text, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line("<enter>", 0, largest_string_size, 1, i++);
    write_center_box_top(largest_string_size, 1, i);

    int x_corner = COLS/2;
    int y_corner = LINES/2 - 2;
    move(y_corner + 4, x_corner);
    char out[1];
    get_input(out, 1);

    replace_arg(line_index, element, program, (uint8_t)out[0]);

    overwrite(saved_state, stdscr);
    refresh();
}

void write_hex_replace(uint32_t line_index, int element, program_t *program){
    WINDOW* saved_state = dupwin(stdscr);

    char* text = "Enter your hex num: (4 byte size)";

    int largest_string_size = (int)strlen(text);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);
    write_center_box_line(text, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line("<enter>", 0, largest_string_size, 1, i++);
    write_center_box_top(largest_string_size, 1, i);

    int x_corner = COLS/2 - largest_string_size/2 + 1;
    int y_corner = LINES/2 - 2;
    move(y_corner + 4, x_corner);
    write_colored("0x", 0);
    attron(A_BOLD);
    char input_str[9];
    input_str[9] = 0;
    get_input_with_exit(input_str, 8);
    attroff(A_BOLD);

    char* end_ptr;
    uint32_t result = (uint32_t)strtol(input_str, &end_ptr, 16);
    if (end_ptr == input_str) {
        return;
    }

    replace_arg(line_index, element, program, result);

    overwrite(saved_state, stdscr);
    refresh();
}

void write_dec_replace(uint32_t line_index, int element, program_t *program){
    WINDOW* saved_state = dupwin(stdscr);

    char* text = "Enter your dec num: (4 byte size, num <= 4 294 967 295)";

    int largest_string_size = (int)strlen(text);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);
    write_center_box_line(text, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line("<enter>", 0, largest_string_size, 1, i++);
    write_center_box_top(largest_string_size, 1, i);

    int x_corner = COLS/2 - (largest_string_size / 2) + 1;
    int y_corner = LINES/2 - 2;
    move(y_corner + 4, x_corner);
    char input_str[11];
    input_str[11] = 0;
    get_input_with_exit(input_str, 10);
    char* end_ptr;
    uint32_t result = (uint32_t)strtol(input_str, &end_ptr, 10);
    if (end_ptr == input_str) {
        return;
    }
    overwrite(saved_state, stdscr);
    if(result > 4294967295){
        write_error_prompt("Number must fit into 4 bytes!");
        return;
    }
    replace_arg(line_index, element, program, result);

    refresh();
}

void write_float_replace(uint32_t line_index, int element, program_t *program){
    WINDOW* saved_state = dupwin(stdscr);

    char* text = "Enter your float:";

    int largest_string_size = (int)strlen(text);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);
    write_center_box_line(text, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line("<enter>", 0, largest_string_size, 1, i++);
    write_center_box_top(largest_string_size, 1, i);

    int x_corner = COLS/2 - (largest_string_size / 2) + 1;
    int y_corner = LINES/2 - 2;
    move(y_corner + 4, x_corner);
    char input_str[21];
    input_str[21] = 0;
    get_input_with_exit(input_str, 20);
    char* end_ptr;
    float result = strtof(input_str, &end_ptr);
    if (end_ptr == input_str) {
        return;
    }
    overwrite(saved_state, stdscr);
    replace_arg(line_index, element, program, transformToIEEE754Float(result));

    refresh();
}

void write_replace_select(uint32_t line_index, int element, program_t *program){
    WINDOW* saved_state = dupwin(stdscr);

    char* c =   "0.) Replace with char.";
    char* hex = "1.) Replace with hex number.";
    char* dec = "2.) Replace with decimal number.";
    char* f =   "3.) Replace with float.";

    char* prompt = "Enter number or 'e' to exit.";

    int largest_string_size = (int)strlen(dec);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);

    write_center_box_line(c, 0, largest_string_size, 1, i++);
    write_center_box_line(hex, 0, largest_string_size, 1, i++);
    write_center_box_line(dec, 0, largest_string_size, 1, i++);
    write_center_box_line(f, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line(prompt, 0, largest_string_size, 1, i++);

    write_center_box_top(largest_string_size, 1, i);

    int selection = get_key_press();

    overwrite(saved_state, stdscr);
    refresh();

    switch (selection){
        case ('0'):
            write_char_replace(line_index, element, program);
            break;
        case ('1'):
            write_hex_replace(line_index, element, program);
            break;
        case ('2'):
            write_dec_replace(line_index, element, program);
            break;
        case('3'):
            write_float_replace(line_index, element, program);
            break;
        default:
            break;
    }
}

uint8_t write_opt_select(){
    WINDOW* saved_state = dupwin(stdscr);

    char* text = "Enter your instructions opt:";

    int largest_string_size = (int)strlen(text);
    int i = 0;

    write_center_box_top(largest_string_size, 1, i++);
    write_center_box_line(text, 0, largest_string_size, 1, i++);
    write_center_box_line("", 0, largest_string_size, 1, i++);
    write_center_box_line("<enter>", 0, largest_string_size, 1, i++);
    write_center_box_top(largest_string_size, 1, i);

    int x_corner = COLS/2 - (2 + largest_string_size/2);
    int y_corner = LINES/2 - 2;
    move(y_corner + 4, x_corner + 14);
    write_colored("0x", 0);
    attron(A_BOLD);
    char input_str[3];
    input_str[3] = 0;
    get_input(input_str, 2);
    attroff(A_BOLD);

    char* end_ptr;
    uint8_t result = (uint8_t)strtol(input_str, &end_ptr, 16);
    if (end_ptr == input_str) {
        return 0x0;
    }

    overwrite(saved_state, stdscr);
    return result;
}

#endif