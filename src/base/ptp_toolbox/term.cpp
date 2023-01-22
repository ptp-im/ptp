/*!
 * toolbox.
 * term.cpp
 *
 * \date 11/30/2019
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#include "toolbox_config.h"

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h> /* for EINTR */
#endif

#include "strings.hpp"
#include "term.h"

#include <cstring>
#include <iostream>

#if defined(HAVE_TERMIOS_H) && defined(HAVE_UNISTD_H)
ssize_t ptp_toolbox::term::_password(const std::string& message, char** pw, size_t sz, int mask, FILE* fp) {
    if (!pw || !sz || !fp)
        return -1; /* validate input   */
#ifdef MAXPW
    if (sz > MAXPW)
        sz = MAXPW;
#endif

    std::cout << message << ": \n";

    if (*pw == nullptr) { /* reallocate if no address */
        void* tmp = realloc(*pw, sz * sizeof **pw);
        if (!tmp)
            return -1;
        memset(tmp, 0, sz); /* initialize memory to 0   */
        *pw = static_cast<char*>(tmp);
    }

    size_t idx = 0; /* index, number of chars in read   */
    int c = 0;

    termios old_kbd_mode; /* orig keyboard settings   */
    termios new_kbd_mode;

    if (tcgetattr(0, &old_kbd_mode)) { /* save orig settings   */
        fprintf(stderr, "%s() error: tcgetattr failed.\n", __func__);
        return -1;
    } /* copy old to new */
    memcpy(&new_kbd_mode, &old_kbd_mode, sizeof(struct termios));

    new_kbd_mode.c_lflag &= ~(ICANON | ECHO); /* new kbd flags */
    new_kbd_mode.c_cc[VTIME] = 0;
    new_kbd_mode.c_cc[VMIN] = 1;
    if (tcsetattr(0, TCSANOW, &new_kbd_mode)) {
        fprintf(stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    /* read chars from fp, mask if valid char specified */
    while (((c = fgetc(fp)) != '\n' && c != EOF && idx < sz - 1) || (idx == sz - 1 && c == 127)) {
        if (c != 127) {
            if (31 < mask && mask < 127) /* valid ascii char */
                fputc(mask, stdout);
            (*pw)[idx++] = c;
        } else if (idx > 0) { /* handle backspace (del)   */
            if (31 < mask && mask < 127) {
                fputc(0x8, stdout);
                fputc(' ', stdout);
                fputc(0x8, stdout);
            }
            (*pw)[--idx] = 0;
        }
    }
    (*pw)[idx] = 0; /* null-terminate   */

    /* reset original keyboard  */
    if (tcsetattr(0, TCSANOW, &old_kbd_mode)) {
        fprintf(stderr, "%s() error: tcsetattr failed.\n", __func__);
        return -1;
    }

    if (idx == sz - 1 && c != '\n') /* warn if pw truncated */
        fprintf(stderr, " (%s() warning: truncated at %zu chars.)\n", __func__, sz - 1);

    return idx; /* number of chars in passwd    */
}
std::string ptp_toolbox::term::prompt_password(const std::string& message, size_t passSize) {
    char* pw = new char[passSize];

    _password(message, &pw, passSize);

    auto out = std::string(pw);
    delete[] pw;
    return out;
}
#endif
bool ptp_toolbox::term::confirm(std::istream& in, std::ostream& out, const std::string& message, bool defValue) {
    std::string res;
    const std::string yesNo = defValue ? "yes" : "no";
    do {
        out << message;
        out << " (yes|no) [" << yesNo << "]: ";
        std::getline(in, res);

    } while (!res.empty() && !ptp_toolbox::strings::equals_icase(res, "yes") &&
             !ptp_toolbox::strings::equals_icase(res, "no") && !ptp_toolbox::strings::equals_icase(res, "y") &&
             !ptp_toolbox::strings::equals_icase(res, "n"));

    if (res.empty()) {
        return defValue;
    }

    return ptp_toolbox::strings::equals_icase(res, "yes") || ptp_toolbox::strings::equals_icase(res, "y");
}
bool ptp_toolbox::term::confirm(const std::string& message, bool defValue) {
    return confirm(std::cin, std::cout, message, defValue);
}
std::string ptp_toolbox::term::prompt(std::istream& in, std::ostream& out, const std::string& message, bool required,
                                  const std::string& defValue) {
    std::string res;
    do {
        out << message;
        if (!defValue.empty()) {
            out << " [" << defValue << "]: ";
        } else {
            out << ": ";
        }

        std::getline(in, res);

    } while ((required && res.empty()));

    if (res.empty()) {
        return defValue;
    }
    return res;
}
std::string ptp_toolbox::term::prompt(const std::string& message, bool required, const std::string& defValue) {
    return prompt(std::cin, std::cout, message, required, defValue);
}
