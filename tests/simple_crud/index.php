<?php require_once "header.php"; ?>

<div class="container m-5">
	<table id="example" class="table table-striped" style="width:100%">
		<thead>
			<tr>
				<th>N°</th>
				<th>First Name</th>
				<th>Last Name</th>
				<th>Profile image</th>
				<th>Operations</th>
			</tr>
		</thead>
		<tbody>
			<?php $data = readfile("db_user.txt"); ?>
			<tr>
				<td>Tiger Nixon</td>
				<td>System Architect</td>
				<td>Edinburgh</td>
				<td>Edinburgh</td>
				<td>
					<button class="btn btn-primary">
					<i class="fa fa-edit"></i>
					</button>
					<button class="btn btn-danger">
					<i class="fa fa-trash"></i>
					</button>
				</td>
		</tbody>
	</table>
</div>
<?php require_once "footer.php"; ?>
