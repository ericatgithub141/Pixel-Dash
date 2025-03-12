// Create a canvas element
const canvas = document.createElement('canvas');
canvas.width = 400;
canvas.height = 300;
canvas.style.border = '1px solid black';
document.body.appendChild(canvas);

const ctx = canvas.getContext('2d');

// Player properties
let playerX = 50;
let playerY = 50;
const playerSize = 50;
let playerXSpeed = 0;
let playerYSpeed = 0;
const playerSpeed = 4;

// Enemy properties
const enemies = [
    { x: 300, y: 200, xSpeed: Math.random() * 4 - 2, ySpeed: Math.random() * 4 - 2 }, // Enemy 1 (random speeds)
    { x: 100, y: 50, xSpeed: Math.random() * 4 - 2, ySpeed: Math.random() * 4 - 2 }  // Enemy 2 (random speeds)
];
const enemySize = 50;

let gameOver = false; // Flag to track game state

// Collectable properties
const collectables = [
    { x: 200, y: 150, collected: false }, // Original collectable
    { x: 100, y: 50, collected: false },  // Add more here!
    { x: 350, y: 250, collected: false }  // Add more here!
];
const collectableSize = 30;

let score = 0; // Keep track of the score

function drawPlayer() {
    ctx.fillStyle = 'red';
    ctx.fillRect(playerX, playerY, playerSize, playerSize);
}

function drawEnemies() {
    ctx.fillStyle = 'blue';
    enemies.forEach(enemy => {
        ctx.fillRect(enemy.x, enemy.y, enemySize, enemySize);
    });
}

function drawCollectable() {
    collectables.forEach(collectable => {
        if (!collectable.collected) {
            ctx.fillStyle = 'yellow';
            ctx.fillRect(collectable.x, collectable.y, collectableSize, collectableSize);
        }
    });
}

function checkCollision() {
    // Player and enemies collision
    enemies.forEach(enemy => {
        const dx = (playerX + playerSize / 2) - (enemy.x + enemySize / 2);
        const dy = (playerY + playerSize / 2) - (enemy.y + enemySize / 2);
        const distanceSq = dx * dx + dy * dy; // Calculate squared distance
        const radiiSumSq = (playerSize / 2 + enemySize / 2) ** 2; // Square of radii sum
        if (distanceSq < radiiSumSq) { // Compare squared values
            gameOver = true;
            alert('Game Over!');
        }
    });

    // Player and collectables collision
    collectables.forEach(collectable => {
        if (!collectable.collected) {
            const collectableDx = (playerX + playerSize / 2) - (collectable.x + collectableSize / 2);
            const collectableDy = (playerY + playerSize / 2) - (collectable.y + collectableSize / 2);
            const collectableDistance = Math.sqrt(collectableDx * collectableDx + collectableDy * collectableDy);
            if (collectableDistance < playerSize / 2 + collectableSize / 2) {
                collectable.collected = true;
                score++;
            }
        }
    });
}

function checkWinCondition() {
    // Check if all collectables have been collected
    return collectables.every(collectable => collectable.collected);
}

function update() {
  if (!gameOver) {
      // Clear the canvas with a black background
      ctx.fillStyle = 'black'; // Set the fill color to black
      ctx.fillRect(0, 0, canvas.width, canvas.height); // Fill the entire canvas

        // Move the player
        playerX += playerXSpeed;
        playerY += playerYSpeed;

        // Keep the player within the canvas bounds
        if (playerX + playerSize > canvas.width) {
            playerX = canvas.width - playerSize;
        } else if (playerX < 0) {
            playerX = 0;
        }
        if (playerY + playerSize > canvas.height) {
            playerY = canvas.height - playerSize;
        } else if (playerY < 0) {
            playerY = 0;
        }

        // Move the enemies
        enemies.forEach(enemy => {
            enemy.x += enemy.xSpeed;
            enemy.y += enemy.ySpeed; // Add vertical movement

            // Make the enemy bounce off the edges
            if (enemy.x + enemySize > canvas.width || enemy.x < 0) {
                enemy.xSpeed = -enemy.xSpeed;
            }
            if (enemy.y + enemySize > canvas.height || enemy.y < 0) {
                enemy.ySpeed = -enemy.ySpeed; // Bounce off top and bottom
            }

            // Randomly change direction
            if (Math.random() < 0.01) { // 1% chance to change direction each frame
                enemy.xSpeed = Math.random() * 4 - 2;
                enemy.ySpeed = Math.random() * 4 - 2;
            }
        });

        // Check for win condition
        if (checkWinCondition()) {
            alert("You Win!");
            gameOver = true; // Stop the game
        }

        // Draw the game objects
        drawPlayer();
        drawEnemies(); // Draw all enemies
        drawCollectable();

         // Display the score (change the text color to white)
         ctx.fillStyle = 'white'; // Set the text color to white
         ctx.font = '24px Arial';
         ctx.fillText('Score: ' + score, 10, 30);

        // Check for collisions AFTER drawing
        checkCollision();

        // Request the next frame for animation
        requestAnimationFrame(update);
    }
}

// Handle keyboard input for player movement
document.addEventListener('keydown', (event) => {
    if (!gameOver) {
        switch (event.key) {
            case 'ArrowUp':
                playerYSpeed = -playerSpeed;
                break;
            case 'ArrowDown':
                playerYSpeed = playerSpeed;
                break;
            case 'ArrowLeft':
                playerXSpeed = -playerSpeed;
                break;
            case 'ArrowRight':
                playerXSpeed = playerSpeed;
                break;
        }
    } else {
        // Reset the game if spacebar is pressed after game over
        if (event.code === 'Space') {
            gameOver = false;
            // Reset player, enemy, and collectable
            playerX = 50;
            playerY = 50;
            enemies[0].x = 300; // Reset enemy 1 position
            enemies[0].y = 200;
            enemies[1].x = 100; // Reset enemy 2 position
            enemies[1].y = 50;
            collectables.forEach((collectable) => {
                collectable.collected = false;
            });
            score = 0; // Reset score
            update();
        }
    }
});

// Stop player movement when key is released
document.addEventListener('keyup', () => {
    playerXSpeed = 0;
    playerYSpeed = 0;
});

// Start the animation
update();