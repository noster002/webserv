<?php require_once "header.php"; ?>
<form action="http://localhost:8082/add.php" method="post" enctype="multipart/form-data" id="myForm">
	<div class="row m-5">
		<div class="col">
			<input type="text" class="form-control" placeholder="First name" aria-label="First name" name="first" required>
		</div>
		<div class="col">
			<input type="text" class="form-control" placeholder="Last name" aria-label="Last name" name="last" required>
		</div>
	</div>
	<div class="row">
		<div class="col-11 m-auto">
			<input class="form-control" type="file" id="formFile" name="img_profile" required>
		</div>
	</div>
	<div class="row m-5">
		<div class="col">
			<button class="btn btn-primary">
				Save
				<i class="fa fa-save" ></i>
			</button>
			<a class="btn btn-danger" href="index.php">
				Cancel
				<i class="fa fa-close"></i>
			</a>		</div>
	</div>
</form>
<?php require_once "footer.php"; ?>