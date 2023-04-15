<?php require_once "header.php"; ?>

<div class="container m-5">
	<?php
	$tmp = file_get_contents("db_user.txt"); 
	$data = explode("\n", $tmp);
	?>
	<table id="example" class="table table-striped" style="width:100%">
		<thead>
			<tr>
				<th>N°</th>
				<th>Profile image</th>
				<th>First Name</th>
				<th>Last Name</th>
				<th>Operations</th>
			</tr>
		</thead>
		<tbody>
			<?php foreach($data as $item) : ?>
				<?php if (!empty($item)): ?>
				<?php 	$row = explode(" ", $item); ?>
				<tr>
					<td class= "m-auto"> <?= ++$i ?></td>
					<?php if (file_exists($row[2])): ?>
						<td class= "m-auto"><img src="<?= $row[2];?>" class="m-auto img-fluid profile-image"/></td>
					<?php else : ?>
						<td class= "m-auto"></td>
					<? endif ?>
					<td class= "m-auto"><?= $row[0]; ?></td>
					<td class= "m-auto"><?= $row[1]; ?></td>
					<td class= "m-auto">
						<button class="btn btn-primary" id= "<?="edit;".$id?>" onclick="put_request(this);">
						<i class="fa fa-edit"></i>
						</button>
						<button class="btn btn-danger" id= "<?=$row[2]?>" onclick="del_request(this);" <?= file_exists($row[2]) ? "" : "disabled"; ?> >
						<i class="fa fa-trash"></i>
						</button>
					</td>
				</tr>
				<?php endif ?>
			<?php endforeach ?> 
		</tbody>
	</table>
</div>
<?php require_once "footer.php"; ?>
