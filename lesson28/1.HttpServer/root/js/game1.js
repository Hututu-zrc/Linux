 window.onload = function() {
      // 自定义弹窗逻辑
      const customAlert = document.getElementById('custom-alert');
      const alertOkButton = document.getElementById('alert-ok-button');
      const countdownElement = document.getElementById('countdown');
      let countdown = 5;
  
      customAlert.style.display = 'block'; // 显示自定义弹窗
  
      // 禁用按钮，并在5秒后启用
      alertOkButton.disabled = true;
      countdownElement.textContent = countdown;
      
      const countdownInterval = setInterval(() => {
          countdown--;
          countdownElement.textContent = countdown;
  
          if (countdown <= 0) {
              clearInterval(countdownInterval);
              alertOkButton.disabled = false;
          }
      }, 1000);
  
      alertOkButton.onclick = function() {
          customAlert.style.display = 'none'; // 隐藏自定义弹窗
      };
  
      // 游戏逻辑
      const gameArea = document.getElementById('game-area');
      const startButton = document.getElementById('start-button');
      const gridWidth = 45;
      const gridHeight = 30;
      let cells = [];
      let snake = [{ x: 25, y: 15 }];
      let direction = { x: 1, y: 0 };
      let nextDirection = { x: 1, y: 0 };
      let food = { x: 0, y: 0 };
      let gameInterval;
      let userControlled = false;
  
      function createGrid() {
          for (let i = 0; i < gridHeight; i++) {
              for (let j = 0; j < gridWidth; j++) {
                  const cell = document.createElement('div');
                  cell.classList.add('grid-cell');
                  cells.push(cell);
                  gameArea.appendChild(cell);
              }
          }
      }
  
      function drawSnake() {
          // 首先清除所有单元格
          cells.forEach(cell => cell.classList.remove('snake', 'snake-head'));
      
          // 显示蛇的身体
          snake.forEach((segment, index) => {
              const cellIndex = segment.y * gridWidth + segment.x;
              cells[cellIndex].classList.add('snake');
              if (index === 0) {
                  cells[cellIndex].classList.add('snake-head'); // 将snake-head类添加到头部段
              }
          });
      }
  
      function drawFood() {
          cells.forEach(cell => cell.classList.remove('food'));
          const index = food.y * gridWidth + food.x;
          cells[index].classList.add('food');
      }
  
      function placeFood() {
          let newFoodPosition;
          do {
              newFoodPosition = {
                  x: Math.floor(Math.random() * gridWidth),
                  y: Math.floor(Math.random() * gridHeight),
              };
          } while (snake.some(segment => segment.x === newFoodPosition.x && segment.y === newFoodPosition.y));
          food = newFoodPosition;
          drawFood();
      }
  
      // 使用BFS计算最短路径的辅助函数
      function findShortestPath(start, target) {
          const queue = [{ pos: start, path: [] }];
          const visited = new Set([`${start.x},${start.y}`]);
  
          while (queue.length > 0) {
              const { pos, path } = queue.shift();
  
              // 检查是否到达目标
              if (pos.x === target.x && pos.y === target.y) {
                  return path;
              }
  
              // 生成相邻位置
              const directions = [{ x: 1, y: 0 }, { x: -1, y: 0 }, { x: 0, y: 1 }, { x: 0, y: -1 }];
              for (const dir of directions) {
                  const newPos = { x: pos.x + dir.x, y: pos.y + dir.y };
  
                  // 检查newPos是否在边界内且未被访问
                  if (newPos.x >= 0 && newPos.x < gridWidth && newPos.y >= 0 && newPos.y < gridHeight && !visited.has(`${newPos.x},${newPos.y}`)) {
                      visited.add(`${newPos.x},${newPos.y}`);
                      queue.push({ pos: newPos, path: [...path, newPos] });
                  }
              }
          }
  
          // 如果找不到路径
          return null;
      }
  
      function moveSnake() {
          if (!userControlled) {
              // 计算到食物的最短路径
              const path = findShortestPath(snake[0], food);
      
              if (path && path.length > 0) {
                  // 根据路径中的下一步确定方向
                  const nextStep = path[0];
                  direction = { x: nextStep.x - snake[0].x, y: nextStep.y - snake[0].y };
              }
          } else {
              direction = nextDirection; // 改变移动方向
          }
      
          // 移动蛇
          const head = { x: snake[0].x + direction.x, y: snake[0].y + direction.y };
      
         // 检查蛇是否撞墙
         if (head.x < 0 || head.x >= gridWidth || head.y < 0 || head.y >= gridHeight) {
             clearInterval(gameInterval);
             customAlert.querySelector('p').textContent = '游戏结束！';
             customAlert.style.display = 'block';
             return;
         }
     
         snake.unshift(head);
     
         // 检查蛇是否吃到食物
         if (head.x === food.x && head.y === food.y) {
             placeFood();
         } else {
             snake.pop();
         }
     
         // 检查蛇是否与自身碰撞
         if (snakeCollision(head)) {
             clearInterval(gameInterval);
             customAlert.querySelector('p').textContent = '游戏结束！';
             customAlert.style.display = 'block';
             return;
         }
     
         drawSnake();
     }
  
     function snakeCollision(head) {
         return snake.slice(1).some(segment => segment.x === head.x && segment.y === head.y);
     }
  
     function changeDirection(event) {
         userControlled = true; // 按键按下后切换到用户控制模式
         switch (event.key) {
             case 'ArrowUp':
             case 'w':
                 if (direction.y === 0) nextDirection = { x: 0, y: -1 };
                 break;
             case 'ArrowDown':
             case 's':
                 if (direction.y === 0) nextDirection = { x: 0, y: 1 };
                 break;
             case 'ArrowLeft':
             case 'a':
                 if (direction.x === 0) nextDirection = { x: -1, y: 0 };
                 break;
             case 'ArrowRight':
             case 'd':
                 if (direction.x === 0) nextDirection = { x: 1, y: 0 };
                 break;
         }
     }
  
     function startGame() {
         clearInterval(gameInterval); // 清除现有的游戏
         snake = [{ x: 25, y: 15 }]; // 初始蛇的位置
         direction = { x: 1, y: 0 }; // 向右移动
         nextDirection = { x: 1, y: 0 };
         userControlled = false; // 重置为自动控制
         placeFood(); // 放置初始食物
         drawSnake(); // 绘制初始蛇
         gameInterval = setInterval(moveSnake, 150); // 设置移速
     }
  
     startButton.addEventListener('click', startGame); // 开始按钮的事件监听器
     document.addEventListener('keydown', changeDirection); // 键盘按键事件监听器
	 
     createGrid(); // 创建游戏网格
  };