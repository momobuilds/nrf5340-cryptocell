# Lesson 3 illustration prompt

Generated with the built-in image-generation tool. No firmware changes.

Use case: scientific-educational.
Create one polished illustrated educational infographic, landscape, with large readable English text, white background, navy lettering, blue/teal process arrows, gold key icons, red only for tampering. Beginner-friendly, minimal jargon, professional enough for an embedded engineering student's supervisor. More illustrated visual explanation than a dense diagram. Title: "Lesson 3: Encrypt, then decrypt". Subtitle: "AES-128-CTR — learning example".
Top two-thirds: five clear illustrated steps left to right, all within a pale-blue rounded boundary labelled "All of this happens on the nRF5340 board".
Step 1 a readable paper card labelled "Plaintext" containing exactly "EMG test packet".
Step 2 a blue encryption machine labelled "Encrypt".
Step 3 a closed envelope of abstract scrambled squares labelled "Ciphertext" with small caption "Unreadable without the key".
Step 4 matching blue machine labelled "Decrypt".
Step 5 an identical readable paper card labelled "Recovered plaintext" containing exactly "EMG test packet" and a green check caption "Exact match".
Show a gold key icon feeding into Encrypt and an identical gold key icon feeding into Decrypt. Join these key icons visually with label "Same key". Similarly show a blue IV token feeding each machine connected with label "Same IV". Clearly distinguish key from IV. Small caption beneath this row: "PSA generates the IV. Decryption needs that same IV."
Bottom third separate light-red panel, clearly titled "What CTR does NOT check". Show an encrypted envelope, then a hand with pencil altering one square without a key, then a decryption machine, then a red damaged readable paper labelled "Changed message". Arrows make cause and effect obvious. Under attacker hand: "Attacker changes a byte". Under the panel: "No key needed to tamper. CTR can decrypt without detecting the change."
Footer: "For Wings: add authentication with AEAD." and smaller "Fixed public test key — not for production."
Accuracy constraints: Current normal round trip is on the board, NOT a PC receiver. Do not depict a built AEAD pipeline or imply authentication exists in CTR. Do not label ciphertext as a hash or randomness. No invented hexadecimal cryptographic output; use symbolic scrambled squares. This is an illustrative lesson, not a hardware photograph. Generous whitespace and no extra paragraphs.

