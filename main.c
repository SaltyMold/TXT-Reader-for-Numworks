#include "eadk.h"

// Déclaration du nom de l'application et du niveau de l'API
const char eadk_app_name[] __attribute__((section(".rodata.eadk_app_name"))) = "TXT_READER";
const uint32_t eadk_api_level  __attribute__((section(".rodata.eadk_api_level"))) = 0;

// Fonction pour dessiner du texte à l'écran
void draw_text(const char *text, eadk_point_t start_point, eadk_color_t fg, eadk_color_t bg) {
  eadk_point_t point = start_point; // Point de départ pour dessiner le texte
  const size_t max_line_length = 33; // Longueur maximale d'une ligne de texte
  char line[max_line_length + 1]; // Buffer pour stocker une ligne de texte
  size_t line_length = 0; // Longueur actuelle de la ligne

  while (*text) {
    size_t word_end = line_length; // Position de la fin du dernier mot
    while (line_length < max_line_length && *text && *text != '\n') {
      if (*text == ' ') {
        word_end = line_length; // Mettre à jour la position de la fin du mot
      }
      line[line_length++] = *text++; // Ajouter le caractère à la ligne
    }

    // Si la ligne est trop longue, revenir au dernier espace
    if (line_length == max_line_length && *text && *text != ' ' && *text != '\n') {
      if (word_end > 0) {
        text -= (line_length - word_end); // Revenir dans le texte
        line_length = word_end; // Revenir à la fin du dernier mot
      }
    }

    line[line_length] = '\0'; // Terminer la ligne avec un caractère nul
    eadk_display_draw_string(line, point, true, fg, bg); // Dessiner la ligne à l'écran
    point.y += 16; // Passer à la ligne suivante (ajuster la valeur si nécessaire)
    line_length = 0; // Réinitialiser la longueur de la ligne

    // Ignorer les espaces supplémentaires
    while (*text == ' ') {
      text++;
    }

    // Passer à la ligne suivante si un caractère de nouvelle ligne est trouvé
    if (*text == '\n') {
      text++;
    }
  }

  // Dessiner la dernière ligne si elle n'est pas vide
  if (line_length > 0) {
    line[line_length] = '\0';
    eadk_display_draw_string(line, point, true, fg, bg);
  }
}

int main(int argc, char * argv[]) {
  // Définir un rectangle couvrant tout l'écran
  eadk_rect_t full_screen_rect = {0, 0, EADK_SCREEN_WIDTH, EADK_SCREEN_HEIGHT};
  eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white); // Remplir l'écran de blanc

  int scroll_offset = 0; // Initialiser le décalage de défilement
  bool refresh_screen = true; // Initialiser le drapeau de rafraîchissement de l'écran

  // Boucle principale pour gérer les entrées du clavier
  while (true) {
    eadk_keyboard_state_t keyboard_state = eadk_keyboard_scan(); // Scanner l'état du clavier
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_back)) {
      break; // Quitter la boucle et terminer le programme si la touche "back" est enfoncée
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_down)) {
      scroll_offset += 16; // Déplacer vers le bas
      refresh_screen = true; // Marquer l'écran pour rafraîchissement
    }
    if (eadk_keyboard_key_down(keyboard_state, eadk_key_up)) {
      scroll_offset -= 16; // Déplacer vers le haut
      if (scroll_offset < 0) {
        scroll_offset = 0; // Empêcher le défilement vers le haut au-delà du début du texte
      }
      refresh_screen = true; // Marquer l'écran pour rafraîchissement
    }

    if (refresh_screen) {
      eadk_display_push_rect_uniform(full_screen_rect, eadk_color_white); // Effacer l'écran
      if (eadk_external_data != NULL && eadk_external_data_size > 0) {
        draw_text(eadk_external_data, (eadk_point_t){0, -scroll_offset}, eadk_color_black, eadk_color_white);
      } else {
        eadk_display_draw_string("No external data", (eadk_point_t){0, 0}, true, eadk_color_black, eadk_color_white);
      }
      refresh_screen = false; // Réinitialiser le drapeau de rafraîchissement de l'écran
    }

    eadk_timing_msleep(100); // Attendre un peu avant de vérifier à nouveau
  }

  return 0; // Terminer le programme
}
