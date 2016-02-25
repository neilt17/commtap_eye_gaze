<button class="board-cell">
  <div class="board-cell-content">
    <?php if (!empty($image)): ?>
    <img src="<?php print $image ?>"/>
    <?php endif; ?>
    <?php if (!empty($display)): ?>
    <div class="display-text"><?php print $display ?></div>
    <?php endif; ?>
    <audio preload="auto">
      <?php foreach ($audio as $audio_src): ?>
      <source src="<?php print $audio_src ?>">
      <?php endforeach ?>
      Your browser does not support this audio feature.
    </audio>
  </div>
</button>